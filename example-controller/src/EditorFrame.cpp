#include "EditorFrame.h"
#include "ofGraphics.h"
#include "ofAppRunner.h"
#include "ofEventUtils.h"

EditorWindow::~EditorWindow()
{
	disableMouseInteraction();
}

void EditorWindow::setup()
{
	enableMouseInteraction();
	scale_ = settings_.min_scale;
}

void EditorWindow::enableMouseInteraction()
{
	if((is_enabled_mouse_interaction_ ^= true) == true) {
		auto &ev = ofEvents();
		ofAddListener(ev.mousePressed, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseReleased, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseMoved, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseDragged, this, &EditorWindow::onMouseEvent);
		ofAddListener(ev.mouseScrolled, this, &EditorWindow::onMouseEvent);
	}
}
void EditorWindow::disableMouseInteraction()
{
	if((is_enabled_mouse_interaction_ ^= true) == false) {
		auto &ev = ofEvents();
		ofRemoveListener(ev.mousePressed, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseReleased, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseMoved, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseDragged, this, &EditorWindow::onMouseEvent);
		ofRemoveListener(ev.mouseScrolled, this, &EditorWindow::onMouseEvent);
	}
}

void EditorWindow::pushMatrix() const
{
	ofPushMatrix();
	ofTranslate(region_.position);
	ofScale(scale_, scale_);
	ofTranslate(-offset_);
}
void EditorWindow::popMatrix() const
{
	ofPopMatrix();
}
void EditorWindow::pushScissor() const
{
	scissor_cache_.is_enabled = glIsEnabled(GL_SCISSOR_TEST);
	if(scissor_cache_.is_enabled) {
		glGetIntegerv(GL_SCISSOR_BOX, scissor_cache_.box);
	}
	else {
		glEnable(GL_SCISSOR_TEST);
	}
	glScissor(region_.getLeft(), ofGetHeight()-region_.getBottom(), region_.getWidth(), region_.getHeight());
}
void EditorWindow::popScissor() const
{
	if(scissor_cache_.is_enabled) {
		glScissor(scissor_cache_.box[0], scissor_cache_.box[1], scissor_cache_.box[2], scissor_cache_.box[3]);
	}
	else {
		glDisable(GL_SCISSOR_TEST);
	}
}


glm::vec2 EditorWindow::getIn(const glm::vec2 &pos) const
{
	return (pos-glm::vec2(region_.position))/scale_ + offset_;
}
glm::vec2 EditorWindow::getOut(const glm::vec2 &pos) const
{
	return (pos - offset_)*scale_ + glm::vec2(region_.position);
}

void EditorWindow::onMouseEvent(ofMouseEventArgs &arg)
{
	auto convert = [&](const glm::vec2 &pos) {
		return getIn(pos) - offset_;
	};
	auto mouse_pos_prev = mouse_pos_;
	mouse_pos_ = convert(arg);
	ofRectangle select_rect(mouse_pos_pressed_, mouse_pos_);
	select_rect.position += glm::vec3(offset_, 0);
	switch(arg.type) {
		case ofMouseEventArgs::Pressed:
			mouse_pos_pressed_ = mouse_pos_;
			switch(arg.button) {
				case OF_MOUSE_BUTTON_LEFT:
					ofNotifyEvent(on_point_selection_, PointSelectionArg{mouse_pos_pressed_+offset_, false}, this);
					break;
				case OF_MOUSE_BUTTON_RIGHT:
					break;
			}
			break;
		case ofMouseEventArgs::Dragged:
			switch(arg.button) {
				case OF_MOUSE_BUTTON_LEFT:
					offset_ -= mouse_pos_ - mouse_pos_prev;
					break;
				case OF_MOUSE_BUTTON_RIGHT:
					ofNotifyEvent(on_rect_selection_, RectSelectionArg{select_rect, false}, this);
					break;
			}
			break;
		case ofMouseEventArgs::Released:
			switch(arg.button) {
				case OF_MOUSE_BUTTON_LEFT:
					if(mouse_pos_ == mouse_pos_pressed_) {
						ofNotifyEvent(on_point_selection_, PointSelectionArg{mouse_pos_pressed_+offset_, true}, this);
					}
					break;
				case OF_MOUSE_BUTTON_RIGHT:
					ofNotifyEvent(on_rect_selection_, RectSelectionArg{select_rect, true}, this);
					break;
			}
			break;
		case ofMouseEventArgs::Scrolled: {
			float scale_prev = scale_;
			scale_ -= arg.scrollY;
			if(scale_ < settings_.min_scale) {
				scale_ = settings_.min_scale;
			}
			offset_ -= mouse_pos_*(1-scale_/scale_prev);
		}	break;
	}
}
