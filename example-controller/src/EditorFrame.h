#pragma once

#include "ofRectangle.h"
#include "ofEvents.h"
#include <glm/vec2.hpp>

class ofTexture;

class EditorWindow
{
public:
	~EditorWindow();
	void setup();

	void pushMatrix() const;
	void popMatrix() const;
	void pushScissor() const;
	void popScissor() const;
	
	void enableMouseInteraction();
	void disableMouseInteraction();
	void toggleMouseInteractionEnabled() { isMouseInteractionEnabled() ? disableMouseInteraction() : enableMouseInteraction(); }
	bool isMouseInteractionEnabled() const { return is_enabled_mouse_interaction_; }

	void setRegion(const ofRectangle &region) { region_ = region; }
	
	glm::vec2 getIn(const glm::vec2 &outer_pos) const;
	glm::vec2 getOut(const glm::vec2 &inner_pos) const;
	float getScale() const { return scale_; }

	void onMouseEvent(ofMouseEventArgs &arg);
	struct PointSelectionArg {
		glm::vec2 pos;
		bool finished;
	};
	struct RectSelectionArg {
		ofRectangle rect;
		bool finished;
	};
	ofEvent<const PointSelectionArg> on_point_selection_;
	ofEvent<const RectSelectionArg> on_rect_selection_;
protected:
	bool is_enabled_mouse_interaction_=false;
	float scale_=1;
	glm::vec2 offset_;
	ofRectangle region_;
	glm::vec2 mouse_pos_;
	glm::vec2 mouse_pos_pressed_;
	
	struct Settings {
		float min_scale = 0.1f;
	} settings_;
private:
	struct ScissorCache {
		bool is_enabled;
		GLint box[4];
	};
	mutable ScissorCache scissor_cache_;
};

