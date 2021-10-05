#pragma once

#include "ofMain.h"
#include "EditorFrame.h"
#include "ofxMapperMesh.h"

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
	ofx::mapper::Mesh mesh_;
	EditorWindow editor_;
	ofTexture texture_;
	
	void onRectSelection(const EditorWindow::RectSelectionArg &arg);
	ofRectangle rect_selection_;
	bool rect_selecting_=false;
};
