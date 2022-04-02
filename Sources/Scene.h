#pragma once

#include <vector>
#include<glm/glm.hpp>
#include "SceneObject.h"
#include "Light.h"
#include "PostprocessUnit.h"
#include "Camera.h"
#include "Stars.h"
#include "VoxelData.h"

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
	float headLightPower = 20000;
	std::vector<SceneObject*> sceneObjects;

	VAO quadVAO;
	VoxelData* voxels = nullptr;

	bool pause = true;
	bool gravitation = false;
	float cameraLastActive = 1000.0f;
	unsigned int contextWidth, contextHeight;
	int partToDraw = -1;
	int noOfPartsToDraw = 16;

	unsigned int quadFBO, quadTexture = 0;
	Shader* quadShader = nullptr;

	Scene(unsigned int contextWidth, unsigned int contextHeight) : contextWidth(contextWidth), contextHeight(contextHeight) {
	}


	void initQuad();
	void initInfinitePlane();
	void initCamera();
	void initMeshesShadersObjects();
	void initQuadFBO();

public:
	~Scene() {
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

	PostprocessUnit* getPostprocessUnit();

	Camera* getCamera();

	VoxelData* getVoxelData();

	float& getLightsPower() {
		return headLightPower;
	}

	void onContextResize(int contextWidth, int contextHeight);
};

