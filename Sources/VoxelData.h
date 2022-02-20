#pragma once

#include "Texture3D.h"
#include "Camera.h"


class VoxelData
{
	Shader* shader = nullptr;	// Don't delete!
	Texture3D* texture = nullptr;
	unsigned int quadVBO, quadVAO;

public :
	VoxelData(Shader* _shader, const char* file);
	~VoxelData();

	void draw(Camera& camera);
};

