#include "EditorFrame.h"
#include <glm/vec2.hpp>
#include "ofTexture.h"
#include "ofGraphics.h"

void EditorWindow::setup()
{
	enable();
	scale_ = 200;
	setMesh(std::make_shared<ofx::mapper::Mesh>());
	mesh_->init({3,3});
}

void EditorWindow::enable()
{
	if((is_enabled_ ^= true) == true) {
		auto &ev = ofEvents();
		ofAddListener(ev.mousePressed, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseReleased, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseMoved, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseDragged, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseScrolled, this, &EditorWindow::onMouseEvent);
	}
}
void EditorWindow::disable()
{
	if((is_enabled_ ^= true) == false) {
		auto &ev = ofEvents();
		ofRemoveListener(ev.mousePressed, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseReleased, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseMoved, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseDragged, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseScrolled, this, &EditorWindow::onMouseEvent);
	}
}

void EditorWindow::draw(ofTexture &texture) const
{
	bool is_scissor_enabled = glIsEnabled(GL_SCISSOR_TEST);
	GLint scissor_box[4];
	if(is_scissor_enabled) {
		glGetIntegerv(GL_SCISSOR_BOX, scissor_box);
	}
	else {
		glEnable(GL_SCISSOR_TEST);
	}
	glScissor(rect_.getLeft(), ofGetHeight()-rect_.getBottom(), rect_.getWidth(), rect_.getHeight());
	ofPushMatrix();
	ofTranslate(rect_.position);
	ofPushMatrix();
	ofScale(scale_, scale_);
	ofTranslate(-offset_);
	texture.bind();
	mesh_->getMesh().draw();
	texture.unbind();
	ofPopMatrix();

	glm::vec2 pos = mouse_pos_ + offset_;
	glm::vec2 dst_index;
	glm::vec2 result;
	bool is_row;
	if(mesh_->getNearestPointOnLine(pos, dst_index, result, is_row)) {
		ofPushStyle();
		ofSetColor(is_row ? ofColor::red : ofColor::yellow);
		ofDrawCircle((result-offset_)*scale_, 10);
		ofPopStyle();
	}
	ofPopMatrix();
	
	if(is_scissor_enabled) {
		glScissor(scissor_box[0], scissor_box[1], scissor_box[2], scissor_box[3]);
	}
	else {
		glDisable(GL_SCISSOR_TEST);
	}
}

void EditorWindow::onMouseEvent(ofMouseEventArgs &arg)
{
	auto mouse_pos_prev = mouse_pos_;
	mouse_pos_ = ((glm::vec2&)arg-rect_.position)/scale_;
	switch(arg.type) {
		case ofMouseEventArgs::Dragged:
			if(arg.button == OF_MOUSE_BUTTON_LEFT) {
				offset_ += mouse_pos_ - mouse_pos_prev;
			}
			else {
				scale_ += mouse_pos_.y - mouse_pos_prev.y;
			}
			break;
			
	}
}
