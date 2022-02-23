#pragma once

#include "Texture3D.h"
#include "Texture1D.h"
#include "Camera.h"


class VoxelData
{
	Shader* shader = nullptr;	// Don't delete!
	Texture3D* voxels = nullptr;
	Texture1D* transfer = nullptr;
	unsigned int quadVBO, quadVAO;
	float maxIntensity;
	float maxAttenuation;

	void exportData();

public :
	VoxelData(Shader* _shader, const char* file);
	~VoxelData();

	void draw(Camera& camera);
};

