#pragma once

#include <vector>
#include<glm/glm.hpp>
#include "SceneObject.h"
#include "Light.h"
#include "Camera.h"
#include "Stars.h"
#include "VoxelData.h"
#include "SkyBox.h"

/*
* Singleton object
*/
class Scene
{
	static Scene* instance;

	glm::vec4 backgroundColor = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
	std::vector<Shader*> shaders;

	Camera* camera = nullptr;
	std::vector<Light> lights;
	float headLightPower = 0.2;
	std::vector<SceneObject*> sceneObjects;

	VAO quadVAO;
	Shader* quadShader;
	VoxelData* voxels = nullptr;
	SkyBox* skybox = nullptr;

	bool pause = true;
	bool gravitation = false;
	float cameraLastActive = 1000.0f;
	unsigned int contextWidth, contextHeight;
	int sliceToDraw = -1;
	int totalNumberOfSlices = std::sqrt(256*256 + 256*256 + 256*256) * 1.1f;

	FBO quadFBO;
	Texture2D* quadColorTexture = nullptr;
	Texture2D* quadDepthTexture = nullptr;

	std::vector<Texture2D*> colorSpecularTextures;

	Scene(unsigned int contextWidth, unsigned int contextHeight) : contextWidth(contextWidth), contextHeight(contextHeight) {
	}


	void initQuadFBO();
	void initQuad();
	void initBasePlate();
	void initCameraAndLights();
	void initMeshesShadersObjects();

public:
	~Scene() {
		serialize();
		destroy();
	}

	static Scene* getInstance();
	static void destroyInstance();

	void init(int contextWidth, int contextHeight);
	void destroy();

	void control(float dt);
	void animate(float dt);
	void draw();

	void togglePause();
	void toggleGravitation();

	Camera* getCamera();

	VoxelData* getVoxelData();

	float& getLightsPower() {
		return headLightPower;
	}

	void onContextResize(int contextWidth, int contextHeight);
	
	void serialize();

};

