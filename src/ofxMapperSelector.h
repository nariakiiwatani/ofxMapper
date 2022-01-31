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
	Selector(){}
	virtual ~Selector();

	Selector(const Selector &src);
	void setMesh(std::shared_ptr<Mesh> mesh);
	std::shared_ptr<Mesh> getMesh() const { return mesh_.lock(); }
	std::vector<Mesh::PointRef> getSelected();
	std::vector<Mesh::ConstPointRef> getSelected() const;
	std::vector<glm::ivec2> getSelectedIndices() const;
	std::size_t getNumSelected() const { return getSelectedIndices().size(); }
	
	std::vector<int> getPointsInside(const ofRectangle &rect) const;
	
	std::size_t numCols() const { return selected_.empty() ? 0 : selected_[0].size(); }
	std::size_t numRows() const { return selected_.size(); }
	std::size_t size() const { return numCols() * numRows(); }
	
	void resize(int num) { resize(num, 1); }
	void resize(int col, int row);
	
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
extern Selector makeNot(const Selector &a);
extern Selector makeAnd(const Selector &a, const Selector &b);
extern Selector makeNand(const Selector &a, const Selector &b);
extern Selector makeOr(const Selector &a, const Selector &b);
extern Selector makeXor(const Selector &a, const Selector &b);

}}
