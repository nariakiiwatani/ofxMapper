#include "ofxMapperMesh.h"
#include "ofLog.h"
#include "ofPolyline.h"

namespace {
static const std::string LOG_TITLE;
}

using namespace ofx::mapper;

namespace io {
#pragma pack(push, 4)
struct Header {
	char ext[4] = {'m','a','p','m'};
	char version[4] = {'0','0','1','0'};
	int col, row, num;
	Header() {
	}
	Header(int col, int row, int num)
	:col(col),row(row),num(num)
	{}
};
struct Point {
	int col, row;
	glm::vec3 v;
	glm::vec3 n;
	glm::vec2 t;
	ofFloatColor c;
	Point(){}
	Point(const Mesh::PointRef &src)
	:col(src.col),row(src.row)
	,v(*src.v),n(*src.n),t(*src.t),c(*src.c)
	{}
};
#pragma pack(pop)
}
namespace {
template<typename T>
void writeTo(std::ostream& os, const T& t) {
	os.write(reinterpret_cast<const char*>(&t), sizeof(T));
}
template<typename T>
void readFrom(std::istream& is, T& t) {
	is.read(reinterpret_cast<char*>(&t), sizeof(T));
}
std::ostream& operator<<(std::ostream& stream, const io::Header &header) {
	writeTo(stream, header.ext);
	writeTo(stream, header.version);
	writeTo(stream, header.col);
	writeTo(stream, header.row);
	writeTo(stream, header.num);
	return stream;
}
std::istream& operator>>(std::istream& stream, io::Header &header) {
	readFrom(stream, header.ext);
	readFrom(stream, header.version);
	readFrom(stream, header.col);
	readFrom(stream, header.row);
	readFrom(stream, header.num);
	return stream;
}
std::ostream& operator<<(std::ostream& stream, const io::Point &point) {
	writeTo(stream, point.col);
	writeTo(stream, point.row);
	writeTo(stream, point.v.x);
	writeTo(stream, point.v.y);
	writeTo(stream, point.v.z);
	writeTo(stream, point.n.x);
	writeTo(stream, point.n.y);
	writeTo(stream, point.n.z);
	writeTo(stream, point.t.x);
	writeTo(stream, point.t.y);
	writeTo(stream, point.c.r);
	writeTo(stream, point.c.g);
	writeTo(stream, point.c.b);
	writeTo(stream, point.c.a);
	return stream;
}
std::istream& operator>>(std::istream& stream, io::Point &point) {
	readFrom(stream, point.col);
	readFrom(stream, point.row);
	readFrom(stream, point.v.x);
	readFrom(stream, point.v.y);
	readFrom(stream, point.v.z);
	readFrom(stream, point.n.x);
	readFrom(stream, point.n.y);
	readFrom(stream, point.n.z);
	readFrom(stream, point.t.x);
	readFrom(stream, point.t.y);
	readFrom(stream, point.c.r);
	readFrom(stream, point.c.g);
	readFrom(stream, point.c.b);
	readFrom(stream, point.c.a);
	return stream;
}
}
void Mesh::save(const std::string &filepath) const
{
	ofFile file(filepath, ofFile::WriteOnly);
	file << io::Header(num_cells_.x, num_cells_.y, (num_cells_.x+1)*(num_cells_.y+1));
	for(int r = 0; r <= num_cells_.y; ++r) {
		for(int c = 0; c <= num_cells_.x; ++c) {
			file << io::Point(const_cast<Mesh*>(this)->getPoint(c, r));
		}
	}
	file.close();
}
void Mesh::load(const std::string &filepath)
{
	ofFile file(filepath);
	io::Header header;
	file >> header;
	resetMesh({header.col, header.row});
	for(int i = 0; i < header.num; ++i) {
		io::Point point;
		file >> point;
		auto ref = getPoint(point.col, point.row);
		*ref.v = point.v;
		*ref.n = point.n;
		*ref.t = point.t;
		*ref.c = point.c;
	}
	file.close();
}

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
		divideColImpl(i, 1/(float)(num_cells.x-i));
	}
	for(int i = 0; i < num_cells.y-1; ++i) {
		divideRowImpl(i, 1/(float)(num_cells.y-i));
	}

	resetIndices();
	ofNotifyEvent(onReset, num_cells, this);
}

void Mesh::resetIndices()
{
	mesh_.clearIndices();
	mesh_.setMode(OF_PRIMITIVE_TRIANGLES);
	int rows = num_cells_.y+1;
	int cols = num_cells_.x+1;
	for(int y = 0; y < rows-1; y++) {
		for(int x = 0; x < cols-1; x++) {
			mesh_.addIndex((x)*rows + y);
			mesh_.addIndex((x)*rows + y+1);
			mesh_.addIndex((x+1)*rows + y);

			mesh_.addIndex((x)*rows + y+1);
			mesh_.addIndex((x+1)*rows + y+1);
			mesh_.addIndex((x+1)*rows + y);
		}
	}
}

void Mesh::divideRow(int index, float offset)
{
	assert(index < num_cells_.y);
	if(offset <= 0 || offset >= 1) {
		ofLogWarning(LOG_TITLE) << "cannot divide by offset outside 0 and 1";
		return;
	}
	divideRowImpl(index, offset);
	resetIndices();
	ofNotifyEvent(onDivideRow, index, this);
}

void Mesh::divideCol(int index, float offset)
{
	assert(index < num_cells_.x);
	if(offset <= 0 || offset >= 1) {
		ofLogWarning(LOG_TITLE) << "cannot divide by offset outside 0 and 1";
		return;
	}
	divideColImpl(index, offset);
	resetIndices();
	ofNotifyEvent(onDivideCol, index, this);
}

void Mesh::deleteRow(int index)
{
	assert(index <= num_cells_.y);
	if(num_cells_.y == 1) {
		ofLogWarning(LOG_TITLE) << "cannot delete a row any more";
		return;
	}
	deleteRowImpl(index);
	resetIndices();
	ofNotifyEvent(onDeleteRow, index, this);
}

void Mesh::deleteCol(int index)
{
	assert(index <= num_cells_.x);
	if(num_cells_.x == 1) {
		ofLogWarning(LOG_TITLE) << "cannot delete a column any more";
		return;
	}
	deleteColImpl(index);
	resetIndices();
	ofNotifyEvent(onDeleteCol, index, this);
}

void Mesh::divideRowImpl(int index, float offset)
{
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
}
void Mesh::divideColImpl(int index, float offset)
{
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
	}}
void Mesh::deleteRowImpl(int index)
{
	auto &vertices = mesh_.getVertices();
	auto &colors = mesh_.getColors();
	auto &texcoords = mesh_.getTexCoords();
	auto &normals = mesh_.getNormals();
	for(int i = mesh_.getNumVertices()-(num_cells_.y+1)+index; i >= 0; i -= (num_cells_.y+1)) {
		vertices.erase(std::begin(vertices)+i);
		colors.erase(std::begin(colors)+i);
		texcoords.erase(std::begin(texcoords)+i);
		normals.erase(std::begin(normals)+i);
	}
	--num_cells_.y;
}
void Mesh::deleteColImpl(int index)
{
	auto &vertices = mesh_.getVertices();
	auto &colors = mesh_.getColors();
	auto &texcoords = mesh_.getTexCoords();
	auto &normals = mesh_.getNormals();
	for(int i = (index+1)*(num_cells_.y+1); i --> index*(num_cells_.y+1);) {
		vertices.erase(std::begin(vertices)+i);
		colors.erase(std::begin(colors)+i);
		texcoords.erase(std::begin(texcoords)+i);
		normals.erase(std::begin(normals)+i);
	}
	--num_cells_.x;
}

Mesh::PointRef Mesh::getPoint(int col, int row)
{
	if(col > num_cells_.x || row > num_cells_.y) return {};
	int index = col*(num_cells_.y+1) + row;
	PointRef ret;
	ret.col = col;
	ret.row = row;
	ret.v = mesh_.getVerticesPointer() + index;
	ret.c = mesh_.getColorsPointer() + index;
	ret.t = mesh_.getTexCoordsPointer() + index;
	ret.n = mesh_.getNormalsPointer() + index;
	return ret;
}

Mesh::ConstPointRef Mesh::getPoint(int col, int row) const
{
	if(col > num_cells_.x || row > num_cells_.y) return {};
	int index = col*(num_cells_.y+1) + row;
	ConstPointRef ret;
	ret.col = col;
	ret.row = row;
	ret.v = mesh_.getVerticesPointer() + index;
	ret.c = mesh_.getColorsPointer() + index;
	ret.t = mesh_.getTexCoordsPointer() + index;
	ret.n = mesh_.getNormalsPointer() + index;
	return ret;
}

bool Mesh::getDividePoint(const glm::vec2 &pos, glm::vec2 &dst) const
{
	for(int row = 0; row < num_cells_.y; ++row) {
		for(int col = 0; col < num_cells_.x; ++col) {
			auto getIndex = [this](int c, int r) { return c*(num_cells_.y+1)+r; };
			ofPolyline quad;
			quad.addVertex(mesh_.getVertex(getIndex(col, row)));
			quad.addVertex(mesh_.getVertex(getIndex(col+1, row)));
			quad.addVertex(mesh_.getVertex(getIndex(col+1, row+1)));
			quad.addVertex(mesh_.getVertex(getIndex(col, row+1)));
			quad.close();
			if(!quad.inside(pos.x, pos.y)) {
				continue;
			}
			glm::vec2 AB = quad[1]-quad[0];
			glm::vec2 AC = quad[2]-quad[0];
			glm::vec2 AD = quad[3]-quad[0];
			glm::vec2 AP = pos-quad[0];
			glm::vec2 CDB = AC-AD-AB;
			auto calc_t = [=](float s) {
				float div = AD.y+s*CDB.y;
				return div == 0 ? 0 : (AP.y-s*AB.y)/div;
			};
			float a = AB.y*CDB.x - AB.x*CDB.y;
			float b = (AB.y*AD.x - AP.y*CDB.x) - (AB.x*AD.y - AP.x*CDB.y);
			float c = AP.x*AD.y - AP.y*AD.x;
			float D = b*b - 4*a*c;
			if(D < 0) {
				ofLogWarning(LOG_TITLE) << "not found";
				return false;
			}
			if(a == 0) {
				float s = b == 0 ? 0 : -c/b;
				float t = calc_t(s);
				dst = {s+col, t+row};
				return ofInRange(s,0,1) && ofInRange(t,0,1);
			}
			float sqrtD = sqrt(D);
			float s = (-b+sqrtD)/(2*a);
			float t = calc_t(s);
			if(ofInRange(s,0,1) && ofInRange(t,0,1)) {
				dst = {s+col, t+row};
				return true;
			}
			s = (-b-sqrtD)/(2*a);
			t = calc_t(s);
			if(ofInRange(s,0,1) && ofInRange(t,0,1)) {
				dst = {s+col, t+row};
				return true;
			}
			ofLogWarning(LOG_TITLE) << "not found";
			return false;
		}
	}
	ofLogWarning(LOG_TITLE) << "not found";
	return false;
}
