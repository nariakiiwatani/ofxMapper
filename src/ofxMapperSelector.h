#pragma once

#include <memory>
#include <vector>
#include <glm/vec2.hpp>
#include "ofxMapperMesh.h"

namespace ofx {
namespace mapper {

class Selector
{
public:
	virtual ~Selector();
	void setMesh(std::shared_ptr<Mesh> mesh);
	std::vector<Mesh::PointRef> getSelected();
	std::vector<Mesh::ConstPointRef> getSelected() const;
	
	std::vector<int> getPointsInside(const ofRectangle &rect) const;
	
	bool isSelected(int index) const;
	bool isSelected(int col, int row) const;
	
	void selectRow(int index);
	void selectCol(int index);
	void selectPoint(int index);
	void selectPoint(int col, int row);
	void selectAll();
	
	void toggleRow(int index);
	void toggleCol(int index);
	void togglePoint(int index);
	void togglePoint(int col, int row);
	void toggleAll();
	
	void clearRow(int index);
	void clearCol(int index);
	void clearPoint(int index);
	void clearPoint(int col, int row);
	void clearAll();
protected:
	std::weak_ptr<Mesh> mesh_;
	std::vector<std::vector<bool>> selected_;
	
	virtual void onResetMesh(const glm::ivec2 &num_cells);
	virtual void onDivideRow(int &index);
	virtual void onDivideCol(int &index);
	virtual void onDeleteRow(int &index);
	virtual void onDeleteCol(int &index);
};
}}
