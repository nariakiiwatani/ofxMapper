#pragma once

#include "ofMesh.h"
#include <glm/vec2.hpp>

namespace ofx {
namespace mapper {
class Mesh
{
public:
	void init(const glm::ivec2 &num_cells = glm::ivec2(1,1)) { resetMesh(num_cells); }
	const ofMesh& getMesh() const { return mesh_; }

	void divideRow(int index, float offset);
	void divideCol(int index, float offset);
private:
	ofMesh mesh_;
	glm::ivec2 num_cells_;
	void resetMesh(const glm::ivec2 &num_cells);
	void resetIndices();
};
}}
