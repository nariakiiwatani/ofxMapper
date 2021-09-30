#include "ofxMapperUpSampler.h"

using namespace ofx::mapper;

ofMesh UpSampler::proc(const Mesh &mesh, float max_vertex_distance) const
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
			auto copyPoint = [](Mesh::ConstPointRef src, Mesh::PointRef dst) {
				*dst.v = *src.v;
				*dst.t = *src.t;
				*dst.n = *src.n;
				*dst.c = *src.c;
			};
			copyPoint(mesh.getPoint(c,r), p[0]);
			copyPoint(mesh.getPoint(c+1,r), p[1]);
			copyPoint(mesh.getPoint(c,r+1), p[2]);
			copyPoint(mesh.getPoint(c+1,r+1), p[3]);
			glm::vec2 diff_max(std::max(glm::distance(*p[0].v, *p[1].v), glm::distance(*p[2].v, *p[3].v)),
							   std::max(glm::distance(*p[0].v, *p[2].v), glm::distance(*p[1].v, *p[3].v)));
			glm::ivec2 num_div = glm::floor(diff_max/max_vertex_distance);
			for(int i = 0; i < num_div.x; ++i) {
				quad.divideCol(i, 1/(float)(num_div.x+1-i));
			}
			for(int i = 0; i < num_div.y; ++i) {
				quad.divideRow(i, 1/(float)(num_div.y+1-i));
			}
			ret.append(quad.getMesh());
		}
	}
	return ret;
}
