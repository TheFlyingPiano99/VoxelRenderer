#pragma once

#include "Texture3D.h"
#include "Texture1D.h"
#include "Camera.h"
#include "Plane.h"
#include "Mesh.h"
#include "BoundingGeometry.h"
#include "Light.h"
#include "TransferFunction.h"
#include "FBO.h"
#include "Animation.h"
#include "GlobalInclude.h"
#include "SkyBox.h"

#define TRANSFER_MODE_COUNT 4

struct BoxEdge {
	glm::vec3 position;
	glm::vec3 direction;
	float length;
};

struct BoundingBox {
	BoxEdge edges[12];
};

class VoxelData
{
	Shader* voxelShader = nullptr;	// Don't delete!
	Shader* voxelHalfAngleShader = nullptr;	// Don't delete!
	Shader* quadShader = nullptr;	// Don't delete!
	Texture3D* voxelTexture = nullptr;
	Texture2D* quadTexture = nullptr;

	VAO* quadVAO;
	FBO enterFBO, exitFBO;
	Texture2D* enterTexture = nullptr;
	Texture2D* exitTexture = nullptr;
	FBO lightFBOs[MAX_LIGHT_COUNT];
	Texture2D* lightTextures[MAX_LIGHT_COUNT] = {nullptr};
	Texture2D* opacityTextures[2] = {nullptr};
	Texture2D* quadDepthTexture = nullptr;

	FBO voxelQuadFBO;

	BoundingBox boundingBox;
	BoundingGeometry boundingGeometry;
	TransferFunction transferFunction;
	Feature* selectedFeature = nullptr;
	std::vector<FeatureGroup> featureGroups;
	FeatureGroup* selectedFeatureGroup;

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
	float exposure, gamma;
	std::string name;
	float boundingGeometryTreshold;
	float transferFloodFillTreshold;
	float STFradius, STFOpacity, STFEmission;

	float noiseScale = 0.2;

	float shininess;
	glm::vec3 specularColor;
	glm::vec3 ambientColor;

	Plane slicingPlane;

	unsigned int shadowSamples;

	const char* transferRegionSelectModes[TRANSFER_MODE_COUNT] = { "Flood fill", "General area", "Single class", "Remove class"};
	const char* currentTransferRegionSelectMode = "Single class";

	Animation* animation = nullptr;
	bool changed = true;

	void exportData(Shader* shader);

	bool readDimensions(const char* path, std::string& name, Dimensions& dimensions);
	void initFBOs(unsigned int contextWidth, unsigned int contextHeight);

	void updateMatrices();

	void drawProxyGeometry(const Camera& camera, const glm::vec3& modelSlicePosition, const glm::vec3& modelSliceNormal);

	void initBoundingBox(Dimensions& dim, BoundingBox& box);

	bool intersectPlane(const BoxEdge& edge, const glm::vec3& planePos, const glm::vec3& planeNormal, glm::vec3& intersection) {
		float t = dot(planePos - edge.position, planeNormal) / dot(edge.direction, planeNormal);
		if (t >= 0 && t < edge.length) {
			intersection = edge.position + edge.direction * t;
			return true;
		}
		return false;
	}

public :
	VoxelData(Shader* _shader, Shader* _voxelHalfAngle, Shader* quadShader, Shader* boundingShader, Shader* _flatColorBoundingShader, Shader* transferShader, VAO* quadVAO, const char* directory, unsigned int contextWidth, unsigned int contextHeight);
	~VoxelData();

	void animate(float dt);
	void control(float dt, bool paused, float cameraLastActive);
	void drawLayer(Camera& camera, Texture2D& targetDepthTeture, Light& light, SkyBox& skybox, unsigned int currentStep, unsigned int stepCount);
	void drawHalfAngleLayer(Camera& camera, Texture2D& targetDepthTeture, Light& light, SkyBox& skybox, unsigned int currentStep, unsigned int stepCount);
	void drawFullWithHalfAngleSlice(Camera& camera, Texture2D& targetDepthTeture, Light& light, SkyBox& skybox);
	void drawQuad(FBO& fbo);
	void drawBoundingGeometry(Camera& camera, std::vector<Light>& lights);
	void drawBoundingGeometryOnScreen(FBO& fbo, Camera& camera, float opacity);
	void drawTransferFunction(FBO& fbo);
	void resetOpacity();

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
	
	float& getNoiseScale() {
		return noiseScale;
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

	glm::vec3 getPosition() {
		return position;
	}

	void mergeVisibleClasses();

	void rotateModelAroundX(float rad);

	void rotateModelAroundY(float rad);

	void rotateModelAroundZ(float rad);

	bool popChanged();

	void setAnimation(Animation* animation);

	Feature* getSelectedFeature();

	TransferFunction* getTransferFunction();

	void setSelectedFeature(const char* name);

	void cycleSelectedFeature();

	void update();

	void redrawSelected();

	void showAll();

	void saveFeatures();

	void loadFeatures();

	void createFeatureGroup();

	std::vector<FeatureGroup>& getFeatureGroups() {
		return featureGroups;
	}

	void addFeatureToFeatureGroup(Feature* feature);

	void addSelectedFeatureToFeatureGroup();

	void removeSelectedFeatureFromFeatureGroup();

	void showSelectedFeatureGroup();

	FeatureGroup* getSelectedFeatureGroup() {
		return selectedFeatureGroup;
	}

	void setSelectedFeatureGroup(const char* name) {
		for (FeatureGroup& group : featureGroups) {
			if (group.name.compare(name) == 0) {
				if (selectedFeatureGroup != &group) {
					selectedFeature = nullptr;
				}
				selectedFeatureGroup = &group;
				if (selectedFeatureGroup != &group) {
					selectedFeature = nullptr;
					showSelectedFeatureGroup();
					update();
				}
				break;
			}
		}
	}
};

