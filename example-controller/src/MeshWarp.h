#pragma once

#include "ofxMapperMesh.h"
#include "ofxMapperSelector.h"
#include "ofxMapperInterpolator.h"
#include "EditorFrame.h"

class MeshWarp
{
public:
	virtual ~MeshWarp();
	void enableInteraction(std::shared_ptr<EditorWindow> editor);
	void disableInteraction();
	
	void setup(const glm::ivec2 &num_cells = glm::ivec2(1,1), const ofRectangle &vert_rect = ofRectangle(0,0,1,1), const ofRectangle &coord_rect = ofRectangle(0,0,1,1));
	void update();
	
	void draw() const;
	void drawWireframe() const;
	void drawGuide() const;
	
	void movePoints(glm::vec2 amount);

private:
	std::weak_ptr<EditorWindow> editor_;
	std::shared_ptr<ofx::mapper::Mesh> mesh_;
	ofx::mapper::Selector selector_;
	ofx::mapper::Interpolator interpolator_;
	
	void onPointSelection(const EditorWindow::PointSelectionArg &arg);
	void onRectSelection(const EditorWindow::RectSelectionArg &arg);
	ofRectangle rect_selection_;
	bool rect_selecting_=false;
	glm::vec2 mesh_pointer_index_;
	glm::vec2 mesh_pointer_;
	bool is_pointer_in_mesh_;
	bool is_pointer_on_mesh_point_;
	bool is_pointer_on_line_, is_pointer_on_row_;

};
