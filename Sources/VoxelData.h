#pragma once

#include "Texture3D.h"
#include "Texture1D.h"
#include "Camera.h"
#include "Plane.h"
#include "Mesh.h"
#include "BoundingGeometry.h"
#include "Light.h"
#include "TransferFunction.h"

#define TRANSFER_MODE_COUNT 4

class VoxelData
{
	Shader* shader = nullptr;	// Don't delete!
	Texture3D* voxels = nullptr;

	VAO* quadVAO;
	unsigned int enterFBO, exitFBO, lightFBO = 0;
	unsigned int enterTexture, exitTexture, lightTexture;

	BoundingGeometry boundingGeometry;
	TransferFunction transferFunction;
	TransferFunction refereceSpatialTransferFunction;

	glm::vec3 scale;
	glm::vec3 position;
	glm::vec3 normal;
	glm::vec3 up;
	glm::vec3 animationEulerAngles;
	glm::vec3 staticEulerAngles;

	glm::mat4 modelMatrix;
	glm::mat4 invModelMatrix;

	float maxIntensity;
	float maxAttenuation;
	Plane plane;
	float exposure, gamma;
	Light light1;
	std::string name;
	float boundingGeometryTreshold;
	float transferFloodFillTreshold;
	float STFradius, STFOpacity, STFEmission;

	unsigned int shadowSamples;

	const char* transferRegionSelectModes[TRANSFER_MODE_COUNT] = { "Flood fill", "General area", "Single class", "Remove class"};
	const char* currentTransferRegionSelectMode = "Single class";

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
	void resetToSTF();
	void resetToDefault();

	std::string getName() {
		return name;
	}

	float& getExposure() {
		return exposure;
	}

	float& getGamma() {
		return gamma;
	}

	float& getBoundingGeometryThreshold() {
		return boundingGeometryTreshold;
	}

	float& getTransferFloodFillThreshold() {
		return transferFloodFillTreshold;
	}

	const char** getTransferRegionSelectModes() {
		return transferRegionSelectModes;
	}

	const char* getCurrentTransferRegionSelectModes() {
		return currentTransferRegionSelectMode;
	}

	void setCurrentTransferRegionSelectModes(const char* str) {
		currentTransferRegionSelectMode = str;
	}

	float& getReferenceTransferFunctionExposure() {
		return refereceSpatialTransferFunction.getExposure();
	}

	float& getReferenceTransferFunctionGamma() {
		return refereceSpatialTransferFunction.getGamma();
	}

	void onContextResize(int width, int height) {
		initFBOs(width, height);
	}

	float& getSTFradius() {
		return STFradius;
	}
	
	float& getSTFOpacity() {
		return STFOpacity;
	}
	float& getSTFEmission() {
		return STFEmission;
	}

	void mergeVisibleClasses();

	void rotateModelAroundX(float rad);

	void rotateModelAroundY(float rad);

	void rotateModelAroundZ(float rad);
};

