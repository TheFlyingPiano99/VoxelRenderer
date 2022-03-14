#pragma once

#include <vector>
#include<glm/glm.hpp>
#include "SceneObject.h"
#include "LightSource.h"
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
	Camera* camera = nullptr;
	std::vector<LightSource*> lights;
	std::vector<Shader*> shaders;
	PostprocessUnit postprocessUnit;

	VAO quadVAO;
	VoxelData* voxels = nullptr;

	bool pause = false;
	bool gravitation = false;
	float cameraLastActive = 1000.0f;
	unsigned int contextWidth, contextHeight;

	Scene(unsigned int contextWidth, unsigned int contextHeight) : contextWidth(contextWidth), contextHeight(contextHeight) {

	}


	void initQuad();
	void initCamera();
	void initMeshesShadersObjects();

	void preShadowRenderPassInit();
	void preGeometryRenderPassInit();


public:
	~Scene() {
		destroy();
	}

	static Scene* getInstance();
	static void destroyInstance();

	void init();
	void destroy();

	void control(float dt);
	void animate(float dt);
	void draw();

	void togglePause();
	void toggleGravitation();

	PostprocessUnit* getPostprocessUnit();

	Camera* getCamera();

	VoxelData* getVoxelData();


};

