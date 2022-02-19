#pragma once

#include <vector>
#include<glm/glm.hpp>
#include "SceneObject.h"
#include "LightSource.h"
#include "PostprocessUnit.h"
#include "Camera.h"
#include "Stars.h"

/*
* Singleton object
*/
class Scene
{
	static Scene* instance;

	glm::vec4 backgroundColor = glm::vec4(0.07f, 0.13f, 0.17f, 1.0f);
	Camera* camera = nullptr;
	std::vector<SceneObject*> objects;
	std::vector<LightSource*> lights;
	std::vector<Mesh*> meshes;
	std::vector<Shader*> objectShaders;
	std::vector<Animation*> animations;
	PostprocessUnit postprocessUnit;

	Planet* planet = nullptr;
	Sun* sun = nullptr;
	Stars* stars = nullptr;

	bool pause = false;
	bool gravitation = false;

	Scene() {

	}


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

	Planet* getPlanet();
	PostprocessUnit* getPostprocessUnit();

	Camera* getCamera();
};

