#include "ofxMapperInterpolator.h"

using namespace ofx::mapper;

void Interpolator::selectCorners()
{
	auto mesh = mesh_.lock();
	if(!mesh) {
		return;
	}
	int cols = mesh->getNumCols();
	int rows = mesh->getNumRows();
	selectPoint(0,0);
	selectPoint(cols,0);
	selectPoint(0,rows);
	selectPoint(cols,rows);
}

void Interpolator::onResetMesh(const glm::ivec2 &num_cells) {
	Selector::onResetMesh(num_cells);
	selectCorners();
}
void Interpolator::onDivideRow(int &index) {
	Selector::onDivideRow(index);
	selectCorners();
}
void Interpolator::onDivideCol(int &index) {
	Selector::onDivideCol(index);
	selectCorners();
}
void Interpolator::onDeleteRow(int &index) {
	Selector::onDeleteRow(index);
	selectCorners();
}
void Interpolator::onDeleteCol(int &index) {
	Selector::onDeleteCol(index);
	selectCorners();
}

void Interpolator::update()
{
	auto mesh = mesh_.lock();
	if(!mesh) {
		return;
	}
	auto gatherPinsX = [&](glm::ivec2 center) {
		int row = center.y;
		std::vector<std::pair<Mesh::PointRef, float>> ret = {
			{ mesh->getPoint(0, row), center.x },
			{ mesh->getPoint(mesh->getNumCols(), row), mesh->getNumCols()-center.x }
		};
		for(int col = center.x; --col >= 0;) {
			if(isSelected(col, row)) {
				ret[0] = {mesh->getPoint(col, row), center.x - col};
				break;
			}
		}
		for(int col = center.x; ++col <= mesh->getNumCols();) {
			if(isSelected(col, row)) {
				ret[1] = {mesh->getPoint(col, row), col - center.x};
				break;
			}
		}
		return ret;
	};
	auto gatherPinsY = [&](glm::ivec2 center) {
		int col = center.x;
		std::vector<std::pair<Mesh::PointRef, float>> ret = {
			{ mesh->getPoint(col, 0), center.y },
			{ mesh->getPoint(col, mesh->getNumRows()), mesh->getNumRows()-center.y }
		};
		for(int row = center.y; --row >= 0;) {
			if(isSelected(col, row)) {
				ret[0] = {mesh->getPoint(col, row), center.y - row};
				break;
			}
		}
		for(int row = center.y; ++row <= mesh->getNumRows();) {
			if(isSelected(col, row)) {
				ret[1] = {mesh->getPoint(col, row), row - center.y};
				break;
			}
		}
		return ret;
	};
	auto interpolate = [](std::vector<std::pair<Mesh::PointRef, float>> pins, Mesh::PointRef dst,  bool use_weight) {
		float sum = 0;
		for(auto &&pin : pins) {
			assert(pin.second > 0);
			pin.second = 1/pin.second;
			sum += pin.second;
		}
		glm::vec3 &v = *dst.v;
		glm::vec2 &t = *dst.t;
		glm::vec3 &n = *dst.n;
		glm::vec4 c;
		v = {0,0,0};
		t = {0,0};
		n = {0,0,0};
		c = {0,0,0,0};
		for(auto &&pin : pins) {
			float weight = use_weight ? pin.second/sum : 1/(float)pins.size();
			v += (*pin.first.v)*weight;
			t += (*pin.first.t)*weight;
			n += (*pin.first.n)*weight;
			auto &&color = *pin.first.c;
			c += glm::vec4(color.r, color.g, color.b, color.a)*weight;
		}
		dst.c->set(c[0], c[1], c[2], c[3]);
	};
	int cols = mesh->getNumCols()+1;
	int rows = mesh->getNumRows()+1;
	for(int c = 1; c < cols-1; ++c) {
		if(!isSelected(c,0)) {
			interpolate(gatherPinsX({c,0}), mesh->getPoint(c,0), true);
		}
		if(!isSelected(c,rows-1)) {
			interpolate(gatherPinsX({c,rows-1}), mesh->getPoint(c,rows-1), true);
		}
	}
	for(int r = 1; r < rows-1; ++r) {
		if(!isSelected(0,r)) {
			interpolate(gatherPinsY({0,r}), mesh->getPoint(0,r), true);
		}
		if(!isSelected(cols-1,r)) {
			interpolate(gatherPinsY({cols-1,r}), mesh->getPoint(cols-1,r), true);
		}
	}
	for(int r = 1; r < rows-1; ++r) {
		for(int c = 1; c < cols-1; ++c) {
			if(selected_[r][c]) {
				continue;
			}
			auto pins_x = gatherPinsX({c,r});
			auto pins_y = gatherPinsY({c,r});
			interpolate({pins_x[0], pins_x[1], pins_y[0], pins_y[1]}, mesh->getPoint(c,r), false);
		}
	}
}

void Interpolator::update(std::shared_ptr<Mesh> dst) const
{
	auto mesh = mesh_.lock();
	if(!mesh) {
		return;
	}
	auto copy = [](const Mesh::PointRef src, Mesh::PointRef dst) {
		dst.col = src.col;
		dst.row = src.row;
		*dst.v = *src.v;
		*dst.t = *src.t;
		*dst.c = *src.c;
		*dst.n = *src.n;
	};
	dst->init({mesh->getNumCols(), mesh->getNumRows()});
	Interpolator proc;
	proc.setMesh(dst);
	int cols = mesh->getNumCols()+1;
	int rows = mesh->getNumRows()+1;
	for(int r = 0; r < rows; ++r) {
		for(int c = 0; c < cols; ++c) {
			if(selected_[r][c]) {
				copy(mesh->getPoint(c,r), dst->getPoint(c,r));
				proc.selectPoint(c,r);
				continue;
			}
		}
	}
	proc.update();
}
