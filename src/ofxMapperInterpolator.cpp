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
	auto gatherPins = [](glm::vec2 center, const std::vector<Mesh::PointRef> &candidates, int num_desired, float distance_max=-1) {
		std::vector<std::pair<const Mesh::PointRef, float>> ret;
		std::multimap<float, Mesh::PointRef> distance_map;
		for(auto &&c : candidates) {
			if(c.col == center.x && c.row == center.y) continue;
			// TODO: centerから見て同じ方向にある点は一番近いものしか使わない的な処理が必要かも
			distance_map.insert(std::make_pair(glm::distance(center, glm::vec2(c.col, c.row)), c));
		}
		float last_distance = 0;
		for(auto &&c : distance_map) {
			if((distance_max >= 0 && c.first > distance_max) || (ret.size() >= num_desired && last_distance != c.first)) {
				break;
			}
			ret.push_back(std::make_pair(c.second, c.first));
			last_distance = c.first;
		}
		return ret;
	};
	auto interpolate = [](std::vector<std::pair<const Mesh::PointRef, float>> pins, Mesh::PointRef dst) {
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
			float weight = pin.second/sum;
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
	auto candidates = getSelected();
	decltype(candidates) cd_x0, cd_x1, cd_y0, cd_y1;
	for(auto &&c : candidates) {
		if(c.col == 0) { cd_x0.push_back(c); }
		if(c.row == 0) { cd_y0.push_back(c); }
		if(c.col == cols-1) { cd_x1.push_back(c); }
		if(c.row == rows-1) { cd_y1.push_back(c); }
	}
	
	for(int r = 0; r < rows; ++r) {
		for(int c = 0; c < cols; ++c) {
			if(selected_[r][c]) {
				continue;
			}
			auto &&pins = r == 0 ? cd_y0
						: r == rows-1 ? cd_y1
						: c == 0 ? cd_x0
						: c == cols-1 ? cd_x1
						: candidates;
			
			auto &&num_desired = (r == 0 || r == rows-1 || c == 0 || c == cols-1) ? 2 : 4;
			auto use_pins = gatherPins(glm::vec2(c,r), pins, num_desired);
			assert(!use_pins.empty());
			interpolate(use_pins, mesh->getPoint(c,r));
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
