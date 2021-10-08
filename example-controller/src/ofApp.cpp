#include "ofApp.h"
#include "ofxMapperSelector.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofLoadImage(texture_, "of.png");
	editor_ = std::make_shared<EditorWindow>();
	editor_->setup();
	editor_->setRegion(ofGetCurrentViewport());
	meshes_.resize(2);
	int i = 0;
	for(auto &&m : meshes_) {
		m = std::make_shared<MeshWarp>();
		m->setup({1,1},{(texture_.getWidth()+100)*i,0,texture_.getWidth(),texture_.getHeight()});
		m->enableInteraction(editor_);
		++i;
	}

}

//--------------------------------------------------------------
void ofApp::update(){
	for(auto &&m : meshes_) {
		m->update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
	editor_->pushScissor();
	editor_->pushMatrix();
	texture_.bind();
	for(auto &&m : meshes_) {
		m->draw();
	}
	texture_.unbind();
	ofPushStyle();
	ofSetColor(ofColor::blue);
	for(auto &&m : meshes_) {
		m->drawWireframe();
	}
	ofPopStyle();
	editor_->popMatrix();

	for(auto &&m : meshes_) {
		m->drawGuide();
	}
	
	editor_->popScissor();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	const float mod_amount = 10;
	glm::vec2 move(0,0);
	switch(key) {
		case OF_KEY_LEFT:
			move.x -= mod_amount;
			break;
		case OF_KEY_RIGHT:
			move.x += mod_amount;
			break;
		case OF_KEY_UP:
			move.y -= mod_amount;
			break;
		case OF_KEY_DOWN:
			move.y += mod_amount;
			break;
	}
	for(auto &&m : meshes_) {
		m->movePoints(move);
	}
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
