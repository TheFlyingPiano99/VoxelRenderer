#pragma once

#include "Texture3D.h"

class VoxelRenderUnit
{

public:
	VoxelRenderUnit() {

	}

	void render(Texture3D& texture, unsigned int FBO);
};

