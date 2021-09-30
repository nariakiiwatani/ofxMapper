#pragma once

#include "ofxMapperSelector.h"

namespace ofx {
namespace mapper {
class Interpolator : public Selector
{
public:
	void update();
	void selectCorners();
protected:
	void onResetMesh(const glm::ivec2 &num_cells) override;
	void onDivideRow(int &index) override;
	void onDivideCol(int &index) override;
	void onDeleteRow(int &index) override;
	void onDeleteCol(int &index) override;
};
}}
