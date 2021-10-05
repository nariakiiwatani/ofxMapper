#pragma once

#include "ofxMapperMesh.h"
#include "ofRectangle.h"
#include "ofEvents.h"
class ofTexture;

class EditorWindow
{
public:
	void setup();
	void setMesh(std::shared_ptr<ofx::mapper::Mesh> mesh) { mesh_ = mesh; }
	
	void enable();
	void disable();
	void toggleEnabled() { isEnabled() ? disable() : enable(); }
	bool isEnabled() const { return is_enabled_; }

	void setWindowArea(const ofRectangle &rect) { rect_ = rect; }
	void draw(ofTexture &texture) const;

	void onMouseEvent(ofMouseEventArgs &arg);
protected:
	std::shared_ptr<ofx::mapper::Mesh> mesh_;
	
	bool is_enabled_=false;
	float scale_=1;
	glm::vec2 offset_;
	ofRectangle rect_;
	glm::vec2 mouse_pos_;
};
