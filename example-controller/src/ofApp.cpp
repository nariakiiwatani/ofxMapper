#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofLoadImage(texture_, "of.png");
	mesh_.init({3,3});
	editor_.setup();
	editor_.setRegion({200,300,500,500});
	ofAddListener(editor_.on_rect_selection_, this, &ofApp::onRectSelection);
}

void ofApp::onRectSelection(const EditorWindow::RectSelectionArg &arg)
{
	rect_selecting_ = !arg.finished;
	rect_selection_ = arg.rect;
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	editor_.beginScissor();
	editor_.pushMatrix();
	texture_.bind();
	mesh_.getMesh().draw();
	texture_.unbind();
	if(rect_selecting_) {
		ofPushStyle();
		ofSetColor(ofColor::gray, 100);
		ofDrawRectangle(rect_selection_);
		ofPopStyle();
	}
	editor_.popMatrix();
	
	glm::vec2 pos = editor_.getIn({ofGetMouseX(), ofGetMouseY()});
	glm::vec2 dst_index;
	glm::vec2 result;
	bool is_row;
	if(mesh_.getNearestPointOnLine(pos, dst_index, result, is_row)) {
		ofPushStyle();
		ofSetColor(is_row ? ofColor::red : ofColor::yellow);
		ofDrawCircle(editor_.getOut(result), 10);
		ofPopStyle();
	}
	editor_.endScissor();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

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
