#pragma once

#include "ofMesh.h"
#include <glm/vec2.hpp>
#include "ofEventUtils.h"

namespace ofx {
namespace mapper {
class Mesh
{
public:
	void init(const glm::ivec2 &num_cells = glm::ivec2(1,1)) { resetMesh(num_cells); }
	const ofMesh& getMesh() const { return mesh_; }
	
	int getNumRows() const { return num_cells_.y; }
	int getNumCols() const { return num_cells_.x; }

	void divideRow(int index, float offset);
	void divideCol(int index, float offset);
	
	void deleteRow(int index);
	void deleteCol(int index);
	ofEvent<int> onDivideRow, onDivideCol, onDeleteRow, onDeleteCol;
	
	struct PointRef {
		int col, row;
		glm::vec3 *v=nullptr;
		glm::vec3 *n=nullptr;
		glm::vec2 *t=nullptr;
		ofFloatColor *c=nullptr;
	};
	PointRef getPoint(int col, int row);
private:
	ofMesh mesh_;
	glm::ivec2 num_cells_;
	void resetMesh(const glm::ivec2 &num_cells);
	void resetIndices();
};
}}
