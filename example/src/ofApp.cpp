#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofDisableArbTex();
	ofLoadImage(texture_, "of.png");
	mesh_ = std::make_shared<ofx::mapper::Mesh>();
	selector_.setMesh(mesh_);
	interpolator_.setMesh(mesh_);
	mesh_->init({4,4});
}

//--------------------------------------------------------------
void ofApp::update(){
	interpolator_.selectAll();
	interpolator_.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofMesh mesh = up_sampler_.proc(*mesh_, 0.1f);
	ofPushMatrix();
	ofScale(ofGetWidth(), ofGetHeight());
	texture_.bind();
	mesh.drawFaces();
	texture_.unbind();
	mesh.drawWireframe();
	ofPopMatrix();
	
	glm::vec2 pos(ofGetMouseX()/(float)ofGetWidth(), ofGetMouseY()/(float)ofGetHeight());
	glm::vec2 dst_index;
	glm::vec2 result;
	bool is_row;
	if(mesh_->getNearestPointOnLine(pos, dst_index, result, is_row)) {
		ofPushStyle();
		ofSetColor(is_row ? ofColor::red : ofColor::yellow);
		glm::vec2 screen_size(ofGetWidth(),ofGetHeight());
		ofDrawCircle(result*screen_size, 10);
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	switch(key) {
		case '0':
			selector_.clearAll();
			break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9': {
			int mod = key-'0';
			selector_.toggleRow(mod);
			selector_.toggleCol(mod);
		}	break;
		case OF_KEY_LEFT: mesh_->deleteCol(1); break;
		case OF_KEY_RIGHT: mesh_->divideCol(0, 0.5f); break;
		case OF_KEY_UP: mesh_->deleteRow(1); break;
		case OF_KEY_DOWN: mesh_->divideRow(0, 0.5f); break;
		case 's': mesh_->save("mesh.mapm"); break;
		case 'l': mesh_->load("mesh.mapm"); break;
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
	float dx = (x - ofGetPreviousMouseX())/(float)ofGetWidth();
	float dy = (y - ofGetPreviousMouseY())/(float)ofGetHeight();
	for(auto &&p : selector_.getSelected()) {
		if(p.v) {
			p.v->x += dx;
			p.v->y += dy;
		}
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	if(button == OF_MOUSE_BUTTON_RIGHT) {
		glm::vec2 pos(x/(float)ofGetWidth(), y/(float)ofGetHeight());
		glm::vec2 div_pos;
		if(mesh_->getIndexOfPoint(pos, div_pos)) {
			int col = div_pos.x;
			int row = div_pos.y;
			mesh_->divideCol(col, div_pos.x-col);
			mesh_->divideRow(row, div_pos.y-row);
			interpolator_.selectPoint(col+1, row+1);
			selector_.selectPoint(col+1, row+1);
		}
	}
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
