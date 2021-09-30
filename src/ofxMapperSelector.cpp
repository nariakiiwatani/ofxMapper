#include "ofxMapperSelector.h"
#include <numeric>

using namespace ofx::mapper;

void Selector::setMesh(std::shared_ptr<Mesh> mesh)
{
	if(auto m = mesh_.lock()) {
		ofRemoveListener(m->onDivideRow, this, &Selector::onDivideRow);
		ofRemoveListener(m->onDivideCol, this, &Selector::onDivideCol);
		ofRemoveListener(m->onDeleteRow, this, &Selector::onDeleteRow);
		ofRemoveListener(m->onDeleteCol, this, &Selector::onDeleteCol);
		ofRemoveListener(m->onReset, this, &Selector::onResetMesh);
	}
	ofAddListener(mesh->onDivideRow, this, &Selector::onDivideRow);
	ofAddListener(mesh->onDivideCol, this, &Selector::onDivideCol);
	ofAddListener(mesh->onDeleteRow, this, &Selector::onDeleteRow);
	ofAddListener(mesh->onDeleteCol, this, &Selector::onDeleteCol);
	ofAddListener(mesh->onReset, this, &Selector::onResetMesh);
	mesh_ = mesh;
	onResetMesh({mesh->getNumCols(), mesh->getNumRows()});
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
	selected_.resize(num_cells.x+1);
	for(auto &row : selected_) {
		row.assign(num_cells.y+1, false);
	}
	clearAll();
}
