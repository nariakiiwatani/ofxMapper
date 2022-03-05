#pragma once

#include "ofMesh.h"
#include <glm/vec2.hpp>
#include "ofEventUtils.h"

namespace ofx {
namespace mapper {
class Selector;
class Mesh
{
public:
	void init(const glm::ivec2 &num_cells = glm::ivec2(1,1), const ofRectangle &vert_rect = ofRectangle(0,0,1,1), const ofRectangle &coord_rect = ofRectangle(0,0,1,1)) { resetMesh(num_cells, vert_rect, coord_rect); }
	void init(const ofMesh &src, const glm::ivec2 &num_cells);
	const ofMesh& getMesh() const { return mesh_; }
	
	void save(const std::string &filepath, Selector *selector=nullptr) const;
	void load(const std::string &filepath, Selector *selector=nullptr);
	void pack(std::ostream &stream, Selector *selector=nullptr) const;
	void unpack(std::istream &stream, Selector *selector=nullptr);
	
	int getNumRows() const { return num_cells_.y; }
	int getNumCols() const { return num_cells_.x; }

	void divideRow(int index, float offset);
	void divideCol(int index, float offset);
	void divideRow(int index, std::initializer_list<float> offsets);
	void divideCol(int index, std::initializer_list<float> offsets);
	void deleteRow(int index);
	void deleteCol(int index);
	ofEvent<int> onDivideRow, onDivideCol, onDeleteRow, onDeleteCol;
	ofEvent<const glm::ivec2> onReset;
	
	struct PointRef {
		int col, row;
		glm::vec3 *v=nullptr;
		glm::vec3 *n=nullptr;
		glm::vec2 *t=nullptr;
		ofFloatColor *c=nullptr;
	};
	struct ConstPointRef {
		int col, row;
		const glm::vec3 *v=nullptr;
		const glm::vec3 *n=nullptr;
		const glm::vec2 *t=nullptr;
		const ofFloatColor *c=nullptr;
	};
	PointRef getPoint(int col, int row);
	ConstPointRef getPoint(int col, int row) const;
	
	bool isInside(const glm::vec2 &pos) const;
	bool getIndexOfPoint(const glm::vec2 &pos, glm::vec2 &dst_findex) const;
	bool getNearestPoint(const glm::vec2 &pos, glm::ivec2 &dst_index, glm::vec2 &result, float max_distance=0) const;
	std::vector<std::pair<glm::ivec2, glm::vec2>> getPointsAround(const glm::vec2 &pos, float max_distance=0) const;
	bool getNearestPointOnLine(const glm::vec2 &pos, glm::vec2 &dst_findex, glm::vec2 &result, bool &is_row, float max_distance=0) const;
	
private:
	ofMesh mesh_;
	glm::ivec2 num_cells_;
	void resetMesh(const glm::ivec2 &num_cells, const ofRectangle &vert_rect, const ofRectangle &coord_rect);
	void resetIndices();
	int getIndex(int col, int row) const { return col*(num_cells_.y+1)+row; };
	
	void divideRowImpl(int index, float offset);
	void divideColImpl(int index, float offset);
	void deleteRowImpl(int index);
	void deleteColImpl(int index);
};
}}
