#pragma once

#include "ofxMapper.h"

namespace ofx {
namespace mapper {
class UpSampler
{
public:
	ofMesh proc(const Mesh &mesh, float max_vertex_distance) const;
};
}}
