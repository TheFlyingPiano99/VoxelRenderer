#pragma once

#include "Texture3D.h"
#include "Texture1D.h"
#include "Camera.h"
#include "Plane.h"
#include "Mesh.h"
#include "BoundingGeometry.h"


class VoxelData
{
	Shader* shader = nullptr;	// Don't delete!
	Texture3D* voxels = nullptr;
	Texture1D* transferFunction = nullptr;
	VAO quadVAO;
	unsigned int enterFBO, exitFBO;
	unsigned int enterTexture, exitTexture;

	BoundingGeometry boundingGeometry;

	float maxIntensity;
	float maxAttenuation;
	glm::vec3 resolution;
	Plane plane;
	float exposure, gamma;
	glm::vec3 lightDir;
	std::string name;

	void exportData();
	static unsigned char* defaultTransferFunction(int resolution);
	static unsigned char* brainOnlyTransferFunction(int resolution);

	bool readDimensions(const char* path, std::string& name, Dimensions& dimensions);
	void initFBOs(unsigned int contextWidth, unsigned int contextHeight);
	void initQuad();

public :
	VoxelData(Shader* _shader, Shader* boundingShader, const char* directory, unsigned int contextWidth, unsigned int contextHeight);
	~VoxelData();

	void animate(float dt);
	void draw(Camera& camera);

	void shiftIntersectionPlane(float delta);
	void rotateIntersectionPlane(float rad);

	std::string getName() {
		return name;
	}
};

