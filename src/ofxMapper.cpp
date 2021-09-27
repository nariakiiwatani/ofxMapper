#include "ofxMapper.h"
#include "ofLog.h"

namespace {
static const std::string LOG_TITLE;
}

using namespace ofx::mapper;

void Mesh::resetMesh(const glm::ivec2 &num_cells)
{
	num_cells_ = glm::ivec2(1,1);
	mesh_.clear();
	mesh_.addVertices({
		{0,0,0},{0,1,0},{1,0,0},{1,1,0}
	});
	mesh_.addColors({
		{1},{1},{1},{1}
	});
	mesh_.addTexCoords({
		{0,0},{0,1},{1,0},{1,1}
	});
	mesh_.addNormals({
		{0,0,1},{0,0,1},{0,0,1},{0,0,1}
	});
	
	for(int i = 0; i < num_cells.x-1; ++i) {
		divideCol(i, 1/(float)(num_cells.x-i));
	}
	for(int i = 0; i < num_cells.y-1; ++i) {
		divideRow(i, 1/(float)(num_cells.y-i));
	}

	resetIndices();
}

void Mesh::resetIndices()
{
	mesh_.clearIndices();
	mesh_.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
	int rows = num_cells_.y+1;
	int cols = num_cells_.x+1;
	for(int y = 0; y < rows-1; y++) {
		if((y&1)==0) {
			for(int x = 0; x < cols; x++) {
				mesh_.addIndex(x*rows + y);
				mesh_.addIndex(x*rows + y+1);
			}
		} else {
			for(int x = cols-1; x >0; x--) {
				mesh_.addIndex(x*rows + y+1);
				mesh_.addIndex((x-1)*rows + y);
			}
		}
	}

	if((rows&1)!=0) mesh_.addIndex(num_cells_.y);
}

void Mesh::divideRow(int index, float offset)
{
	assert(index < num_cells_.y);
	if(offset <= 0 || offset >= 1) {
		ofLogWarning(LOG_TITLE) << "cannot divide by offset outside 0 and 1";
		return;
	}
	
	++num_cells_.y;
	int cols = num_cells_.x+1;
	int rows = num_cells_.y+1;
	int num_vertices = mesh_.getNumVertices()+cols;
	auto &vertices = mesh_.getVertices();
	auto &colors = mesh_.getColors();
	auto &texcoords = mesh_.getTexCoords();
	auto &normals = mesh_.getNormals();
	vertices.resize(num_vertices);
	colors.resize(num_vertices);
	texcoords.resize(num_vertices);
	normals.resize(num_vertices);
	for(int x = cols; x--> 0;) {
		for(int y = rows; y--> 0;) {
			int dst = x*rows+y;
			int src = x*(rows-1)+(y-(y>index?1:0));
			if(y == index+1) {
				vertices[dst] = glm::mix(vertices[src], vertices[src+1], offset);
				colors[dst] = colors[src].getLerped(colors[src+1], offset);
				texcoords[dst] = glm::mix(texcoords[src], texcoords[src+1], offset);
				normals[dst] = glm::mix(normals[src], normals[src+1], offset);
			}
			else {
				vertices[dst] = vertices[src];
				colors[dst] = colors[src];
				texcoords[dst] = texcoords[src];
				normals[dst] = normals[src];
			}
		}
	}
	resetIndices();
}

void Mesh::divideCol(int index, float offset)
{
	assert(index < num_cells_.x);
	if(offset <= 0 || offset >= 1) {
		ofLogWarning(LOG_TITLE) << "cannot divide by offset outside 0 and 1";
		return;
	}

	++num_cells_.x;
	int cols = num_cells_.x+1;
	int rows = num_cells_.y+1;
	int num_vertices = mesh_.getNumVertices()+rows;
	auto &vertices = mesh_.getVertices();
	auto &colors = mesh_.getColors();
	auto &texcoords = mesh_.getTexCoords();
	auto &normals = mesh_.getNormals();
	vertices.resize(num_vertices);
	colors.resize(num_vertices);
	texcoords.resize(num_vertices);
	normals.resize(num_vertices);
	for(int x = cols; x--> 0;) {
		int src_x = (x-(x>index?1:0));
		if(x == index+1) {
			for(int y = rows; y--> 0;) {
				int dst = x*rows+y;
				int src = src_x*rows+y;
				vertices[dst] = glm::mix(vertices[src], vertices[src+rows], offset);
				colors[dst] = colors[src].getLerped(colors[src+rows], offset);
				texcoords[dst] = glm::mix(texcoords[src], texcoords[src+rows], offset);
			}
		}
		else {
			for(int y = rows; y--> 0;) {
				int dst = x*rows+y;
				int src = src_x*rows+y;
				vertices[dst] = vertices[src];
				colors[dst] = colors[src];
				texcoords[dst] = texcoords[src];
				normals[dst] = normals[src];
			}
		}
	}
	resetIndices();
}

Mesh::PointRef Mesh::getPoint(int col, int row)
{
	if(col > num_cells_.x || row > num_cells_.y) return {};
	int index = col*(num_cells_.y+1) + row;
	PointRef ret;
	ret.v = mesh_.getVerticesPointer() + index;
	ret.c = mesh_.getColorsPointer() + index;
	ret.t = mesh_.getTexCoordsPointer() + index;
	ret.n = mesh_.getNormalsPointer() + index;
	return ret;
}
