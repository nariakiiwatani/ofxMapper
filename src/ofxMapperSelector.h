#pragma once

#include <memory>
#include <unordered_set>
#include <glm/vec2.hpp>
#include "ofxMapper.h"

namespace std {
template <> struct hash<glm::ivec2> {
	hash() = default;
	hash(const hash &h):v(h.v) {}
	hash(hash &&h):v(h.v) {}
	~hash() = default;
	hash& operator=(const hash &h) {
		v = h.v;
		return *this;
	}
	hash& operator=(hash &&h) {
		v = h.v;
		return *this;
	}
	size_t operator()(glm::ivec2 key) const {
		return key.x << 16 | key.y;
	}
	glm::ivec2 v;
};
}
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
	
	virtual void onDivideRow(int &index){}
	virtual void onDivideCol(int &index){}
	virtual void onDeleteRow(int &index){}
	virtual void onDeleteCol(int &index){}
protected:
	std::weak_ptr<Mesh> mesh_;
	std::unordered_set<glm::ivec2> selected_;
};
}}
