#pragma once

#include "Texture3D.h"
#include "Texture1D.h"
#include "Camera.h"
#include "Plane.h"
#include "Mesh.h"
#include "BoundingGeometry.h"
#include "Light.h"
#include "TransferFunction.h"

class VoxelData
{
	Shader* shader = nullptr;	// Don't delete!
	Texture3D* voxels = nullptr;

	VAO* quadVAO;
	unsigned int enterFBO, exitFBO, lightFBO;
	unsigned int enterTexture, exitTexture, lightTexture;

	BoundingGeometry boundingGeometry;
	TransferFunction transferFunction;
	TransferFunction refereceSpatialTransferFunction;

	glm::vec3 scale;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 up;
	glm::vec3 eulerAngles;

	glm::mat4 modelMatrix;
	glm::mat4 invModelMatrix;

	float maxIntensity;
	float maxAttenuation;
	Plane plane;
	float exposure, gamma;
	Light light1;
	std::string name;
	float treshold;

	unsigned int shadowSamples;

	void exportData();

	bool readDimensions(const char* path, std::string& name, Dimensions& dimensions);
	void initFBOs(unsigned int contextWidth, unsigned int contextHeight);

	void updateMatrices();

public :
	VoxelData(Shader* _shader, Shader* boundingShader, Shader* transferShader, VAO* quadVAO, const char* directory, unsigned int contextWidth, unsigned int contextHeight);
	~VoxelData();

	void animate(float dt);
	void optimize(float dt, bool paused, float cameraLastActive);
	void draw(Camera& camera);

	void shiftIntersectionPlane(float delta);
	void rotateIntersectionPlane(float rad);
	void selectTransferFunctionRegion(double xpos, double ypos);

	std::string getName() {
		return name;
	}
};

