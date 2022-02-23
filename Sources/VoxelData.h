#pragma once

#include "Texture3D.h"
#include "Texture1D.h"
#include "Camera.h"
#include "Plane.h"


class VoxelData
{
	Shader* shader = nullptr;	// Don't delete!
	Texture3D* voxels = nullptr;
	Texture1D* transferFunction = nullptr;
	unsigned int quadVBO, quadVAO;
	float maxIntensity;
	float maxAttenuation;
	glm::vec3 resolution;
	Plane plane;
	float exposure, gamma;
	glm::vec3 lightDir;


	void exportData();
	static unsigned char* defaultTransferFunction(int resolution);
	static unsigned char* brainOnly(int resolution);

public :
	VoxelData(Shader* _shader, const char* file);
	~VoxelData();

	void animate(float dt);
	void draw(Camera& camera);

	void shiftIntersectionPlane(float delta);
	void rotateIntersectionPlane(float rad);
};

