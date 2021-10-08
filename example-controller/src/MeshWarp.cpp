#include "MeshWarp.h"
#include "ofGraphics.h"

MeshWarp::~MeshWarp()
{
	disableInteraction();
}

void MeshWarp::setup(const glm::ivec2 &num_cells, const ofRectangle &vert_rect, const ofRectangle &coord_rect)
{
	mesh_ = std::make_shared<ofx::mapper::Mesh>();
	mesh_->init(num_cells, vert_rect, coord_rect);
//	mesh_->load("data.mapm");
	selector_.setMesh(mesh_);
	interpolator_.setMesh(mesh_);
}

void MeshWarp::enableInteraction(std::shared_ptr<EditorWindow> editor)
{
	disableInteraction();
	editor_ = editor;
	if(editor) {
		ofAddListener(editor->on_point_selection_, this, &MeshWarp::onPointSelection);
		ofAddListener(editor->on_rect_selection_, this, &MeshWarp::onRectSelection);
	}
}

void MeshWarp::disableInteraction()
{
	if(auto e = editor_.lock()) {
		ofRemoveListener(e->on_point_selection_, this, &MeshWarp::onPointSelection);
		ofRemoveListener(e->on_rect_selection_, this, &MeshWarp::onRectSelection);
	}
}

void MeshWarp::update()
{
	auto editor = editor_.lock();
	if(editor) {
		float find_distance = 10;
		glm::vec2 pos = editor->getIn({ofGetMouseX(), ofGetMouseY()});
		is_pointer_on_mesh_point_ = false;
		is_pointer_on_line_ = false;
		is_pointer_in_mesh_ = false;
		glm::ivec2 dst_index;
		glm::vec2 dst_findex;
		glm::vec2 result;
		if(mesh_->getNearestPoint(pos, dst_index, result, find_distance/editor->getScale())) {
			is_pointer_on_mesh_point_ = true;
			is_pointer_in_mesh_ = true;
			mesh_pointer_ = result;
			mesh_pointer_index_ = {dst_index};
		}
		else if(mesh_->getNearestPointOnLine(pos, dst_findex, result, is_pointer_on_row_, find_distance/editor->getScale())) {
			is_pointer_on_line_ = true;
			is_pointer_in_mesh_ = true;
			mesh_pointer_ = result;
			mesh_pointer_index_ = dst_findex;
		}
		else if(mesh_->getIndexOfPoint(pos, dst_findex)) {
			is_pointer_in_mesh_ = true;
			mesh_pointer_ = pos;
			mesh_pointer_index_ = dst_findex;
		}
		else {
		}
	}
	interpolator_.update();
}

void MeshWarp::draw() const
{
	mesh_->getMesh().draw();
}
void MeshWarp::drawWireframe() const
{
	mesh_->getMesh().drawWireframe();
}

void MeshWarp::drawGuide() const
{
	auto editor = editor_.lock();
	if(editor) {
		if(rect_selecting_) {
			ofRectangle rect(editor->getOut(rect_selection_.getTopLeft()),
							 editor->getOut(rect_selection_.getBottomRight())
							 );
			
			ofPushStyle();
			ofSetColor(ofColor::gray, 100);
			ofDrawRectangle(rect);
			ofPopStyle();
		}
		ofPushStyle();
		ofSetColor(ofColor::red);
		for(auto &&p : selector_.getSelected()) {
			if(p.v) {
				ofDrawCircle(editor->getOut(glm::vec2(*p.v)), 10);
			}
		}
		ofPopStyle();
		if(rect_selecting_) {
			ofx::mapper::Selector sel;
			sel.setMesh(mesh_);
			auto inside = sel.getPointsInside(rect_selection_);
			ofPushStyle();
			ofSetColor(ofColor::yellow);
			for(auto &&i : inside) {
				if(!interpolator_.isSelected(i)) {
					continue;
				}
				auto pos = mesh_->getPoint(i/(mesh_->getNumRows()+1), i%(mesh_->getNumRows()+1)).v;
				ofDrawCircle(editor->getOut(glm::vec2(*pos)), 5);
			}
			ofPopStyle();
		}
		float find_distance = 10;
		if(is_pointer_on_mesh_point_) {
			glm::vec2 result = glm::vec2(*mesh_->getPoint(mesh_pointer_index_.x, mesh_pointer_index_.y).v);
			bool selectable = interpolator_.isSelected(mesh_pointer_index_.x, mesh_pointer_index_.y);
			ofPushStyle();
			ofSetColor(selectable ? ofColor::yellow : ofColor::blue);
			ofDrawCircle(editor->getOut(result), find_distance);
			ofPopStyle();
		}
		else if(is_pointer_on_line_) {
			glm::vec2 result = mesh_pointer_;
			ofPushStyle();
			ofSetColor(ofColor::blue);
			ofDrawCircle(editor->getOut(result), find_distance);
			ofPopStyle();
		}
		else if(is_pointer_in_mesh_) {
			glm::vec2 result = mesh_pointer_;
			ofPushStyle();
			ofSetColor(ofColor::green);
			ofDrawCircle(editor->getOut(result), find_distance);
			ofPopStyle();
		}
		else {
		}
	}
}

void MeshWarp::onPointSelection(const EditorWindow::PointSelectionArg &arg)
{
	if(arg.finished) {
		if(is_pointer_on_mesh_point_) {
			if(ofGetKeyPressed(OF_KEY_COMMAND)) {
				interpolator_.togglePoint(mesh_pointer_index_.x, mesh_pointer_index_.y);
				interpolator_.selectCorners();
			}
			else if(interpolator_.isSelected(mesh_pointer_index_.x, mesh_pointer_index_.y)) {
				if(ofGetKeyPressed(OF_KEY_SHIFT)) {
					selector_.selectPoint(mesh_pointer_index_.x, mesh_pointer_index_.y);
				}
				else if(ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL)) {
					interpolator_.selectPoint(mesh_pointer_index_.x, mesh_pointer_index_.y);
					selector_.togglePoint(mesh_pointer_index_.x, mesh_pointer_index_.y);
				}
				else {
					selector_.clearAll();
					selector_.selectPoint(mesh_pointer_index_.x, mesh_pointer_index_.y);
				}
			}
		}
		else {
			if(ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL) || ofGetKeyPressed(OF_KEY_SHIFT)) {
			}
			else {
				selector_.clearAll();
				if(is_pointer_on_line_) {
					if(is_pointer_on_row_) {
						int col_index = mesh_pointer_index_.x;
						int row_index = mesh_pointer_index_.y;
						mesh_->divideCol(col_index, mesh_pointer_index_.x - col_index);
						interpolator_.selectPoint(col_index+1, row_index);
					}
					else {
						int col_index = mesh_pointer_index_.x;
						int row_index = mesh_pointer_index_.y;
						mesh_->divideRow(row_index, mesh_pointer_index_.y - row_index);
						interpolator_.selectPoint(col_index, row_index+1);
					}
				}
				else if(is_pointer_in_mesh_) {
					int col_index = mesh_pointer_index_.x;
					int row_index = mesh_pointer_index_.y;
					mesh_->divideCol(col_index, mesh_pointer_index_.x - col_index);
					mesh_->divideRow(row_index, mesh_pointer_index_.y - row_index);
					interpolator_.selectPoint(col_index+1, row_index+1);
				}
			}
		}
	}
}

void MeshWarp::onRectSelection(const EditorWindow::RectSelectionArg &arg)
{
	rect_selecting_ = !arg.finished;
	rect_selection_ = arg.rect;
	if(arg.finished) {
		ofx::mapper::Selector sel;
		sel.setMesh(mesh_);
		auto inside = sel.getPointsInside(rect_selection_);
		if(ofGetKeyPressed(OF_KEY_SHIFT)) {
			for(auto &&i : inside) {
				if(!interpolator_.isSelected(i)) {
					continue;
				}
				selector_.selectPoint(i);
			}
		}
		else if(ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL)) {
			for(auto &&i : inside) {
				if(!interpolator_.isSelected(i)) {
					continue;
				}
				selector_.togglePoint(i);
			}
		}
		else {
			selector_.clearAll();
			for(auto &&i : inside) {
				if(!interpolator_.isSelected(i)) {
					continue;
				}
				selector_.selectPoint(i);
			}
		}
	}
}

void MeshWarp::movePoints(glm::vec2 amount)
{
	for(auto &&p : selector_.getSelected()) {
		p.v->x += amount.x;
		p.v->y += amount.y;
	}
}
