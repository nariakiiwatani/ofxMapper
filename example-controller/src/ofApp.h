#pragma once

#include "ofMain.h"
#include "EditorFrame.h"
#include "ofxMapperMesh.h"
#include "ofxMapperSelector.h"
#include "ofxMapperInterpolator.h"

class ofApp : public ofBaseApp{
	
public:
	void setup();
	void update();
	void draw();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
private:
	std::shared_ptr<ofx::mapper::Mesh> mesh_;
	std::shared_ptr<ofx::mapper::Mesh> mesh_interpolated_;
	EditorWindow editor_;
	ofTexture texture_;
	ofx::mapper::Selector selector_;
	ofx::mapper::Interpolator interpolator_;
	
	void onPointSelection(const EditorWindow::PointSelectionArg &arg);
	void onRectSelection(const EditorWindow::RectSelectionArg &arg);
	ofRectangle rect_selection_;
	bool rect_selecting_=false;
	glm::vec2 mesh_pointer_index_;
	bool is_pointer_on_mesh_point_;
	bool is_pointer_on_line_, is_pointer_on_row_;
};
