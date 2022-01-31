#include "ofxMapperUpSampler.h"

using namespace ofx::mapper;

ofMesh UpSampler::proc(const Mesh &mesh, float max_vertex_distance, const ofRectangle *use_area) const
{
	ofMesh ret;
	for(int r = 0; r < mesh.getNumRows(); ++r) {
		for(int c = 0; c < mesh.getNumCols(); ++c) {
			Mesh quad;
			quad.init({1,1});
			Mesh::PointRef p[4] = {
				quad.getPoint(0,0), quad.getPoint(1,0),
				quad.getPoint(0,1), quad.getPoint(1,1)
			};
			Mesh::ConstPointRef src[4] = {
				mesh.getPoint(c,r), mesh.getPoint(c+1,r),
				mesh.getPoint(c,r+1), mesh.getPoint(c+1,r+1)
			};
			if(use_area) {
				ofRectangle aabb(*src[0].v, 1, 1);
				for(int i = 0; i < 4; ++i) {
					aabb.growToInclude(*src[i].v);
				}
				if(!use_area->intersects(aabb)) {
					continue;
				}
			}
			auto copyPoint = [](Mesh::ConstPointRef src, Mesh::PointRef dst) {
				*dst.v = *src.v;
				*dst.t = *src.t;
				*dst.n = *src.n;
				*dst.c = *src.c;
			};
			for(int i = 0; i < 4; ++i) {
				copyPoint(src[i], p[i]);
			}
			glm::vec2 diff_max(std::max(glm::distance(*p[0].v, *p[1].v), glm::distance(*p[2].v, *p[3].v)),
							   std::max(glm::distance(*p[0].v, *p[2].v), glm::distance(*p[1].v, *p[3].v)));
			bool div = false;
			if(diff_max.x > max_vertex_distance) {
				quad.divideCol(0, 0.5f);
				div = true;
			}
			if(diff_max.y > max_vertex_distance) {
				quad.divideRow(0, 0.5f);
				div = true;
			}
			if(div) {
				ret.append(proc(quad, max_vertex_distance, use_area));
			}
			else {
				ret.append(quad.getMesh());
			}
			/*
			glm::ivec2 num_div = glm::floor(diff_max/max_vertex_distance);
			for(int i = 0; i < num_div.x; ++i) {
				quad.divideCol(i, 1/(float)(num_div.x+1-i));
			}
			for(int i = 0; i < num_div.y; ++i) {
				quad.divideRow(i, 1/(float)(num_div.y+1-i));
			}
			ret.append(quad.getMesh());
			 */
		}
	}
	return ret;
}
