#pragma once

#include <memory>
#include <vector>
#include <glm/vec2.hpp>
#include "ofxMapper.h"

namespace ofx {
namespace mapper {

class Selector
{
public:
	void setMesh(std::shared_ptr<Mesh> mesh);
	std::vector<Mesh::PointRef> getSelected();
	
	void selectRow(int index);
	void selectCol(int index);
	void selectPoint(int col, int row);
	void selectAll();
	
	void toggleRow(int index);
	void toggleCol(int index);
	void togglePoint(int col, int row);
	void toggleAll();
	
	void clearRow(int index);
	void clearCol(int index);
	void clearPoint(int col, int row);
	void clearAll();
protected:
	std::weak_ptr<Mesh> mesh_;
	std::vector<std::vector<bool>> selected_;
	
	void onResetMesh(const glm::ivec2 &num_cells);
	void onDivideRow(int &index);
	void onDivideCol(int &index);
	void onDeleteRow(int &index);
	void onDeleteCol(int &index);
};
}}
