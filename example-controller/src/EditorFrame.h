#pragma once

#include "ofRectangle.h"
#include "ofEvents.h"
#include <glm/vec2.hpp>

class ofTexture;

class EditorWindow
{
public:
	void setup();

	void pushMatrix() const;
	void popMatrix() const;
	void beginScissor() const;
	void endScissor() const;
	
	void enableMouseInteraction();
	void disableMouseInteraction();
	void toggleMouseInteractionEnabled() { isMouseInteractionEnabled() ? disableMouseInteraction() : enableMouseInteraction(); }
	bool isMouseInteractionEnabled() const { return is_enabled_mouse_interaction_; }

	void setRegion(const ofRectangle &region) { region_ = region; }
	
	glm::vec2 getIn(const glm::vec2 &outer_pos) const;
	glm::vec2 getOut(const glm::vec2 &inner_pos) const;

	void onMouseEvent(ofMouseEventArgs &arg);
	struct RectSelectionArg {
		ofRectangle rect;
		bool finished;
	};
	ofEvent<const RectSelectionArg> on_rect_selection_;
protected:
	bool is_enabled_mouse_interaction_=false;
	float scale_=1;
	glm::vec2 offset_;
	ofRectangle region_;
	glm::vec2 mouse_pos_;
	glm::vec2 mouse_pos_pressed_;
	
	struct Settings {
		float min_scale = 100;
	} settings_;
private:
	struct ScissorCache {
		bool is_enabled;
		GLint box[4];
	};
	mutable ScissorCache scissor_cache_;
};
