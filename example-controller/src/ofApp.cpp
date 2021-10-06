#include "ofApp.h"
#include "ofxMapperSelector.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofLoadImage(texture_, "of.png");
	mesh_ = std::make_shared<ofx::mapper::Mesh>();
	mesh_->init({5,3});
	editor_.setup();
	editor_.setRegion({200,300,500,500});
	selector_.setMesh(mesh_);
	ofAddListener(editor_.on_rect_selection_, this, &ofApp::onRectSelection);
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
				selector_.selectPoint(i);
			}
		}
		else if(ofGetKeyPressed(OF_KEY_COMMAND) || ofGetKeyPressed(OF_KEY_CONTROL)) {
			for(auto &&i : inside) {
				selector_.togglePoint(i);
			}
		}
		else {
			selector_.clearAll();
			for(auto &&i : inside) {
				selector_.selectPoint(i);
			}
		}
	}
}

//--------------------------------------------------------------
void ofApp::update(){
}

//--------------------------------------------------------------
void ofApp::draw(){
	editor_.beginScissor();
	editor_.pushMatrix();
	texture_.bind();
	mesh_->getMesh().draw();
	texture_.unbind();
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
			auto pos = mesh_->getPoint(i/(mesh_->getNumRows()+1), i%(mesh_->getNumRows()+1)).v;
			ofDrawCircle(editor_.getOut(glm::vec2(*pos)), 5);
		}
		ofPopStyle();
	}
	glm::vec2 pos = editor_.getIn({ofGetMouseX(), ofGetMouseY()});
	glm::vec2 dst_index;
	glm::vec2 result;
	bool is_row;
	if(mesh_->getNearestPointOnLine(pos, dst_index, result, is_row)) {
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
