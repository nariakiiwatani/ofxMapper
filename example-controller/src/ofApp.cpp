#include "ofApp.h"
#include "ofxMapperSelector.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofLoadImage(texture_, "of.png");
	mesh_ = std::make_shared<ofx::mapper::Mesh>();
	mesh_->init({1,1});
	mesh_->load("data.mapm");
	editor_.setup();
	editor_.setRegion({200,300,500,500});
	selector_.setMesh(mesh_);
	interpolator_.setMesh(mesh_);
	ofAddListener(editor_.on_point_selection_, this, &ofApp::onPointSelection);
	ofAddListener(editor_.on_rect_selection_, this, &ofApp::onRectSelection);
	mesh_interpolated_ = std::make_shared<ofx::mapper::Mesh>();
}

void ofApp::onPointSelection(const EditorWindow::PointSelectionArg &arg)
{
	if(arg.finished) {
		if(is_pointer_on_mesh_point_) {
			if(ofGetKeyPressed(OF_KEY_COMMAND)) {
				cout << "do" << endl;
				interpolator_.togglePoint(mesh_pointer_index_.x, mesh_pointer_index_.y);
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
			else {
				int col_index = mesh_pointer_index_.x;
				int row_index = mesh_pointer_index_.y;
				mesh_->divideCol(col_index, mesh_pointer_index_.x - col_index);
				mesh_->divideRow(row_index, mesh_pointer_index_.y - row_index);
				interpolator_.selectPoint(col_index+1, row_index+1);
			}
		}
	}
}
void ofApp::onRectSelection(const EditorWindow::RectSelectionArg &arg)
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

//--------------------------------------------------------------
void ofApp::update(){
	interpolator_.update(mesh_interpolated_);
}

//--------------------------------------------------------------
void ofApp::draw(){
	editor_.pushScissor();
	editor_.pushMatrix();
	texture_.bind();
	mesh_interpolated_->getMesh().draw();
	texture_.unbind();
	ofPushStyle();
	ofSetColor(ofColor::blue);
	mesh_interpolated_->getMesh().drawWireframe();
	ofSetColor(ofColor::white);
	mesh_->getMesh().drawWireframe();
	ofPopStyle();
	if(rect_selecting_) {
		ofPushStyle();
		ofSetColor(ofColor::gray, 100);
		ofDrawRectangle(rect_selection_);
		ofPopStyle();
	}
	editor_.popMatrix();
	
	ofPushStyle();
	ofSetColor(ofColor::red);
	for(auto &&p : selector_.getSelected()) {
		if(p.v) {
			ofDrawCircle(editor_.getOut(glm::vec2(*p.v)), 10);
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
			ofDrawCircle(editor_.getOut(glm::vec2(*pos)), 5);
		}
		ofPopStyle();
	}
	float find_distance = 10;
	glm::vec2 pos = editor_.getIn({ofGetMouseX(), ofGetMouseY()});
	is_pointer_on_mesh_point_ = false;
	is_pointer_on_line_ = false;
	glm::ivec2 dst_index;
	glm::vec2 result;
	if(mesh_->getNearestPoint(pos, dst_index, result, find_distance/editor_.getScale())) {
		bool selectable = interpolator_.isSelected(dst_index.x, dst_index.y);
		ofPushStyle();
		ofSetColor(selectable ? ofColor::yellow : ofColor::blue);
		ofDrawCircle(editor_.getOut(result), find_distance);
		ofPopStyle();
		is_pointer_on_mesh_point_ = true;
		mesh_pointer_index_ = {dst_index};
	}
	else if(mesh_->getNearestPointOnLine(pos, mesh_pointer_index_, result, is_pointer_on_row_, find_distance/editor_.getScale())) {
		ofPushStyle();
		ofSetColor(ofColor::blue);
		ofDrawCircle(editor_.getOut(result), find_distance);
		ofPopStyle();
		is_pointer_on_line_ = true;
	}
	else if(mesh_->getIndexOfPoint(pos, mesh_pointer_index_)) {
		ofPushStyle();
		ofSetColor(ofColor::green);
		ofDrawCircle(editor_.getOut(pos), find_distance);
		ofPopStyle();
	}
	editor_.popScissor();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	const float mod_amount = 0.01f;
	switch(key) {
		case OF_KEY_LEFT:
			for(auto &&p : selector_.getSelected()) {
				p.v->x -= mod_amount;
			}
			break;
		case OF_KEY_RIGHT:
			for(auto &&p : selector_.getSelected()) {
				p.v->x += mod_amount;
			}
			break;
		case OF_KEY_UP:
			for(auto &&p : selector_.getSelected()) {
				p.v->y -= mod_amount;
			}
			break;
		case OF_KEY_DOWN:
			for(auto &&p : selector_.getSelected()) {
				p.v->y += mod_amount;
			}
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
