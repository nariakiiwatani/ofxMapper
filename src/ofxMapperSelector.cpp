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
	}
	ofAddListener(mesh->onDivideRow, this, &Selector::onDivideRow);
	ofAddListener(mesh->onDivideCol, this, &Selector::onDivideCol);
	ofAddListener(mesh->onDeleteRow, this, &Selector::onDeleteRow);
	ofAddListener(mesh->onDeleteCol, this, &Selector::onDeleteCol);
	mesh_ = mesh;
}

std::vector<Mesh::PointRef> Selector::getSelected()
{
	auto mesh = mesh_.lock();
	if(!mesh) return {};
	return std::accumulate(std::begin(selected_), std::end(selected_), std::vector<Mesh::PointRef>{}, [mesh](std::vector<Mesh::PointRef> &sum, const glm::ivec2 &cell) {
		sum.push_back(mesh->getPoint(cell.x, cell.y));
		return sum;
	});
	return {mesh->getPoint(0,0)};
}


void Selector::selectRow(int index)
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int cols = mesh->getNumCols();
	for(int i = 0; i <= cols; ++i) {
		selectPoint(i, index);
	}
}
void Selector::selectCol(int index)
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int rows = mesh->getNumRows();
	for(int i = 0; i <= rows; ++i) {
		selectPoint(index, i);
	}
}
void Selector::selectPoint(int col, int row)
{
	selected_.insert({col, row});
}
void Selector::selectAll()
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int cols = mesh->getNumCols();
	int rows = mesh->getNumRows();
	for(int r = 0; r <= rows; ++r) {
		for(int c = 0; c <= cols; ++c) {
			selectPoint(c, r);
		}
	}
}

void Selector::toggleRow(int index)
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int cols = mesh->getNumCols();
	for(int i = 0; i <= cols; ++i) {
		togglePoint(i, index);
	}
}
void Selector::toggleCol(int index)
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int rows = mesh->getNumRows();
	for(int i = 0; i <= rows; ++i) {
		togglePoint(index, i);
	}
}
void Selector::togglePoint(int col, int row)
{
	auto result = selected_.insert({col, row});
	if(!result.second) {
		selected_.erase(result.first);
	}
}
void Selector::toggleAll()
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int cols = mesh->getNumCols();
	int rows = mesh->getNumRows();
	for(int r = 0; r <= rows; ++r) {
		for(int c = 0; c <= cols; ++c) {
			togglePoint(c, r);
		}
	}
}

void Selector::clearRow(int index)
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int cols = mesh->getNumCols();
	for(int i = 0; i <= cols; ++i) {
		clearPoint(i, index);
	}
}
void Selector::clearCol(int index)
{
	auto mesh = mesh_.lock();
	if(!mesh) return;
	int rows = mesh->getNumRows();
	for(int i = 0; i <= rows; ++i) {
		clearPoint(index, i);
	}
}
void Selector::clearPoint(int col, int row)
{
	auto found = std::find(std::begin(selected_), std::end(selected_), glm::ivec2{col, row});
	if(found == std::end(selected_)) {
		return;
	}
	selected_.erase(found);
}
void Selector::clearAll()
{
	selected_.clear();
}

