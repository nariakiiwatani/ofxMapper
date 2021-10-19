#include "ofxMapperSelector.h"
#include "ofxMapperMesh.h"
#include <numeric>

namespace ofx {
namespace mapper {
Selector makeNot(const Selector &a)
{
	Selector ret;
	ret.resize(a.numCols(), a.numRows());
	for(int i = 0; i < a.size(); ++i) {
		a.isSelected(i) ? ret.clearPoint(i) : ret.selectPoint(i);
	}
	return ret;
}
Selector makeAnd(const Selector &a, const Selector &b)
{
	assert(a.size() == b.size());
	Selector ret;
	ret.resize(a.numCols(), a.numRows());
	for(int i = 0; i < a.size(); ++i) {
		a.isSelected(i) && b.isSelected(i) ? ret.selectPoint(i) : ret.clearPoint(i);
	}
	return ret;
}
Selector makeNand(const Selector &a, const Selector &b)
{
	return makeNot(makeAnd(a,b));
}
Selector makeOr(const Selector &a, const Selector &b)
{
	assert(a.size() == b.size());
	Selector ret;
	ret.resize(a.numCols(), a.numRows());
	for(int i = 0; i < a.size(); ++i) {
		a.isSelected(i) || b.isSelected(i) ? ret.selectPoint(i) : ret.clearPoint(i);
	}
	return ret;
}
Selector makeXor(const Selector &a, const Selector &b)
{
	assert(a.size() == b.size());
	Selector ret;
	ret.resize(a.numCols(), a.numRows());
	for(int i = 0; i < a.size(); ++i) {
		a.isSelected(i) != b.isSelected(i) ? ret.selectPoint(i) : ret.clearPoint(i);
	}
	return ret;
}
}}

using namespace ofx::mapper;

Selector::Selector(const Selector &src)
{
	resize(src.numCols(), src.numRows());
	for(int i = 0; i < src.size(); ++i) {
		src.isSelected(i) ? clearPoint(i) : selectPoint(i);
	}
}

Selector::~Selector()
{
	setMesh(nullptr);
}


void Selector::setMesh(std::shared_ptr<Mesh> mesh)
{
	if(auto m = mesh_.lock()) {
		ofRemoveListener(m->onDivideRow, this, &Selector::onDivideRow);
		ofRemoveListener(m->onDivideCol, this, &Selector::onDivideCol);
		ofRemoveListener(m->onDeleteRow, this, &Selector::onDeleteRow);
		ofRemoveListener(m->onDeleteCol, this, &Selector::onDeleteCol);
		ofRemoveListener(m->onReset, this, &Selector::onResetMesh);
	}
	mesh_ = mesh;
	if(mesh) {
		ofAddListener(mesh->onDivideRow, this, &Selector::onDivideRow);
		ofAddListener(mesh->onDivideCol, this, &Selector::onDivideCol);
		ofAddListener(mesh->onDeleteRow, this, &Selector::onDeleteRow);
		ofAddListener(mesh->onDeleteCol, this, &Selector::onDeleteCol);
		ofAddListener(mesh->onReset, this, &Selector::onResetMesh);
		onResetMesh({mesh->getNumCols(), mesh->getNumRows()});
	}
}

void Selector::resize(int col, int row)
{
	selected_.resize(row);
	for(auto &row : selected_) {
		row.resize(col);
	}
}

std::vector<Mesh::PointRef> Selector::getSelected()
{
	auto mesh = mesh_.lock();
	if(!mesh) return {};
	std::vector<Mesh::PointRef> ret;
	for(int r = 0; r < selected_.size(); ++r) {
		auto &row = selected_[r];
		for(int c = 0; c < row.size(); ++c) {
			if(row[c]) {
				ret.push_back(mesh->getPoint(c,r));
			}
		}
	}
	return ret;
}

std::vector<Mesh::ConstPointRef> Selector::getSelected() const
{
	std::shared_ptr<const Mesh> mesh = mesh_.lock();
	if(!mesh) return {};
	std::vector<Mesh::ConstPointRef> ret;
	for(int r = 0; r < selected_.size(); ++r) {
		auto &row = selected_[r];
		for(int c = 0; c < row.size(); ++c) {
			if(row[c]) {
				ret.push_back(mesh->getPoint(c,r));
			}
		}
	}
	return ret;
}

std::vector<glm::ivec2> Selector::getSelectedIndices() const
{
	std::vector<glm::ivec2> ret;
	for(int r = 0; r < selected_.size(); ++r) {
		auto &row = selected_[r];
		for(int c = 0; c < row.size(); ++c) {
			if(row[c]) {
				ret.push_back({c,r});
			}
		}
	}
	return ret;
}

std::vector<int> Selector::getPointsInside(const ofRectangle &rect) const
{
	auto mesh = mesh_.lock();
	if(!mesh) return {};
	std::vector<int> ret;
	for(int r = 0; r < selected_.size(); ++r) {
		auto &row = selected_[r];
		for(int c = 0; c < row.size(); ++c) {
			auto &&pos = mesh->getPoint(c, r);
			if(rect.inside(pos.v->x, pos.v->y)) {
				ret.push_back(c*selected_.size()+r);
			}
		}
	}
	return ret;
}

bool Selector::isSelected(int index) const
{
	if(selected_.empty()) {
		return false;
	}
	int row = index%selected_.size();
	int col = index/selected_.size();
	return isSelected(col, row);
}
bool Selector::isSelected(int col, int row) const
{
	if(selected_.empty()
	   || selected_.size() <= row
	   || selected_[row].size() <= col
	   ) {
		return false;
	}
	return selected_[row][col];
}

void Selector::selectRow(int index)
{
	if(selected_.empty()) {
		return;
	}
	int cols = selected_[0].size();
	for(int i = 0; i < cols; ++i) {
		selectPoint(i, index);
	}
}
void Selector::selectCol(int index)
{
	int rows = selected_.size();
	for(int i = 0; i < rows; ++i) {
		selectPoint(index, i);
	}
}
void Selector::selectPoint(int index)
{
	if(selected_.empty()) {
		return;
	}
	int row = index%selected_.size();
	int col = index/selected_.size();
	selectPoint(col, row);
}
void Selector::selectPoint(int col, int row)
{
	if(selected_.empty()
	   || selected_.size() <= row
	   || selected_[row].size() <= col
	   ) {
		return;
	}
	selected_[row][col] = true;
}
void Selector::selectAll()
{
	for(int r = 0; r < selected_.size(); ++r) {
		for(int c = 0; c <= selected_[r].size(); ++c) {
			selectPoint(c, r);
		}
	}
}

void Selector::toggleRow(int index)
{
	if(selected_.empty()) {
		return;
	}
	int cols = selected_[0].size();
	for(int i = 0; i < cols; ++i) {
		togglePoint(i, index);
	}
}
void Selector::toggleCol(int index)
{
	int rows = selected_.size();
	for(int i = 0; i < rows; ++i) {
		togglePoint(index, i);
	}
}
void Selector::togglePoint(int index)
{
	if(selected_.empty()) {
		return;
	}
	int row = index%selected_.size();
	int col = index/selected_.size();
	togglePoint(col, row);
}
void Selector::togglePoint(int col, int row)
{
	if(selected_.empty()
	   || selected_.size() <= row
	   || selected_[row].size() <= col
	   ) {
		return;
	}
	bool prev = selected_[row][col];
	selected_[row][col] = !prev;
}
void Selector::toggleAll()
{
	for(int r = 0; r < selected_.size(); ++r) {
		for(int c = 0; c <= selected_[r].size(); ++c) {
			togglePoint(c, r);
		}
	}
}

void Selector::clearRow(int index)
{
	if(selected_.empty()) {
		return;
	}
	int cols = selected_[0].size();
	for(int i = 0; i < cols; ++i) {
		clearPoint(i, index);
	}
}
void Selector::clearCol(int index)
{
	int rows = selected_.size();
	for(int i = 0; i < rows; ++i) {
		clearPoint(index, i);
	}
}
void Selector::clearPoint(int index)
{
	if(selected_.empty()) {
		return;
	}
	int row = index%selected_.size();
	int col = index/selected_.size();
	clearPoint(col, row);
}
void Selector::clearPoint(int col, int row)
{
	if(selected_.empty()
	   || selected_.size() <= row
	   || selected_[row].size() <= col
	   ) {
		return;
	}
	selected_[row][col] = false;
}
void Selector::clearAll()
{
	for(int r = 0; r < selected_.size(); ++r) {
		for(int c = 0; c <= selected_[r].size(); ++c) {
			clearPoint(c, r);
		}
	}
}

void Selector::onDivideRow(int &index)
{
	if(selected_.empty() || selected_.size() <= index+1) {
		return;
	}
	selected_.insert(std::begin(selected_)+index+1, std::vector<bool>(selected_[index+1].size(), false));
}
void Selector::onDivideCol(int &index)
{
	for(auto &r : selected_) {
		if(r.size() <= index+1) {
			continue;
		}
		r.insert(std::begin(r)+index+1, false);
	}
}
void Selector::onDeleteRow(int &index)
{
	if(selected_.empty() || selected_.size() <= index) {
		return;
	}
	selected_.erase(std::begin(selected_)+index);
}
void Selector::onDeleteCol(int &index)
{
	for(auto &r : selected_) {
		if(r.size() <= index) {
			continue;
		}
		r.erase(std::begin(r)+index);
	}
}

void Selector::onResetMesh(const glm::ivec2 &num_cells)
{
	resize(num_cells.x+1, num_cells.y+1);
	clearAll();
}
