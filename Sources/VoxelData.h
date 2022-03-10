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

	glm::vec3 scale;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 up;

	glm::mat4 modelMatrix;
	glm::mat4 invModelMatrix;

	float maxIntensity;
	float maxAttenuation;
	glm::vec3 resolution;
	Plane plane;
	float exposure, gamma;
	glm::vec3 lightPosition;
	glm::vec3 lightIntensity;
	std::string name;

	unsigned int shadowSamples;

	void exportData();
	static unsigned char* defaultTransferFunction(int resolution);
	static unsigned char* skinTransferFunction(int resolution);
	static unsigned char* brainOnlyTransferFunction(int resolution);

	bool readDimensions(const char* path, std::string& name, Dimensions& dimensions);
	void initFBOs(unsigned int contextWidth, unsigned int contextHeight);
	void initQuad();

	void updateModelMatrix();

public :
	VoxelData(Shader* _shader, Shader* boundingShader, const char* directory, unsigned int contextWidth, unsigned int contextHeight);
	~VoxelData();

	void animate(float dt);
	void optimize(float dt, bool paused, float cameraLastActive);
	void draw(Camera& camera);

	void shiftIntersectionPlane(float delta);
	void rotateIntersectionPlane(float rad);

	std::string getName() {
		return name;
	}
};

