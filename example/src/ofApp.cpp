#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofLoadImage(texture_, "of.png");
	mesh_.init();
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){
	ofPushMatrix();
	ofScale(ofGetWidth(), ofGetHeight());
	texture_.bind();
	mesh_.getMesh().drawFaces();
	texture_.unbind();
	mesh_.getMesh().drawWireframe();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	int num = key-'0';
	if(num >= 0 && num <= 9) {
		mesh_.init({num+1, num+1});
	}
	else {
		switch(key) {
			case OF_KEY_LEFT: mesh_.deleteCol(1); break;
			case OF_KEY_UP: mesh_.deleteRow(1); break;
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	*mesh_.getPoint(0, 0).v = {x/(float)ofGetWidth(), y/(float)ofGetHeight(), 0};
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
