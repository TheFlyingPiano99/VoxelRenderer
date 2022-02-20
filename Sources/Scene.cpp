#include "Scene.h"
#include "GlobalInclude.h"
#include "ControlActionManager.h"
#include "AssetManager.h"
#include "Planet.h"
#include "Moon.h"

#include "TestObject.h"

//------- Ignore this ----------
#include<filesystem>
namespace fs = std::filesystem;
//------------------------------


// Vertices coordinates
Vertex vertices[] =
{ //               COORDINATES           /            COLORS          /           TexCoord         /       NORMALS         //
	Vertex{glm::vec3(-1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec3(-1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec3(1.0f, 0.0f,  1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec3(-0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f, -0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f,  0.1f)},
	Vertex{glm::vec3(-0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f, -0.1f)},
	Vertex{glm::vec3(0.1f,  0.1f,  0.1f)}
};

GLuint lightIndices[] =
{
	0, 1, 2,
	0, 2, 3,
	0, 4, 7,
	0, 7, 3,
	3, 7, 6,
	3, 6, 2,
	2, 6, 5,
	2, 5, 1,
	1, 5, 4,
	1, 4, 0,
	4, 5, 6,
	4, 6, 7
};




Scene* Scene::instance = nullptr;

void Scene::initCamera()
{
	camera = new Camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.2f, 2.0f));
}

void Scene::initMeshesShadersObjects()
{
	/*
* I'm doing this relative path thing in order to centralize all the resources into one folder and not
* duplicate them between tutorial folders. You can just copy paste the resources from the 'Resources'
* folder and then give a relative path from this folder to whatever resource you want to get to.
* Also note that this requires C++17, so go to Project Properties, C/C++, Language, and select C++17
*/


	// Texture data
	Texture2D textures[]
	{
		Texture2D((AssetManager::getInstance()->getTextureFolderPath().append("planks.png") ).c_str(), "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE),
		Texture2D((AssetManager::getInstance()->getTextureFolderPath().append("planksSpec.png")).c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE)
	};

	// Generates Shader object using shaders default.vert and default.frag
	// Store mesh data in vectors for the mesh
	std::vector <Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
	std::vector <GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
	std::vector <Texture2D> tex(textures, textures + sizeof(textures) / sizeof(Texture2D));

	std::vector <Vertex> lightVerts(lightVertices, lightVertices + sizeof(lightVertices) / sizeof(Vertex));
	std::vector <GLuint> lightInd(lightIndices, lightIndices + sizeof(lightIndices) / sizeof(GLuint));

	glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec3 lightPos = glm::vec3(0.5f, 0.5f, 0.5f);
	glm::mat4 lightModel = glm::mat4(1.0f);
	lightModel = glm::translate(lightModel, lightPos);

	//Light of cube:
	unsigned int pointLightIndex = 0;
	PointLight* sunLight = new PointLight(pointLightIndex++, glm::vec3(0, 0, 0), glm::vec3(10, 10, 10));
	sunLight->setAttenuation(0.0001, 0.1, 0);
	lights.push_back(sunLight);

	
	//Static point lights:
	PointLight* staticPointLight = new PointLight(pointLightIndex++, glm::vec3(0.5f, 0.25f, 0), glm::vec3(1, 3, 1));
	staticPointLight->setAttenuation(6, 2, 1);
	lights.push_back(staticPointLight);

	staticPointLight = new PointLight(pointLightIndex++, glm::vec3(0, 0.25f, 0.5f), glm::vec3(3, 1, 1));
	staticPointLight->setAttenuation(6, 2, 1);
	lights.push_back(staticPointLight);

	staticPointLight = new PointLight(pointLightIndex++, glm::vec3(0.5f, 0.25f, 0.5f), glm::vec3(1, 1, 3));
	staticPointLight->setAttenuation(6, 2, 1);
	lights.push_back(staticPointLight);
	

	//Directional light:
	DirectionalLight* dirLight = new DirectionalLight(normalize(glm::vec3(1, 1, 1)), glm::vec3(0.001f, 0.001f, 0.001f));
	lights.push_back(dirLight);

	glm::vec3 objectPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::mat4 objectModel = glm::mat4(1.0f);
	objectModel = glm::translate(objectModel, objectPos);

	Shader* shaderProgram = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("default.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("default.frag").c_str()
	);
	Shader* lightShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("light.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("light.frag").c_str()
	);

	objectShaders.push_back(shaderProgram);
	objectShaders.push_back(lightShader);

	
	// PLANET THINGY;
	planet = new Planet(shaderProgram);
	objects.push_back(planet);


	SceneObject* moon = new Moon(shaderProgram);
	Animation* moonAnim = new GoAround(-50.0f, 0.00005f, glm::vec3(0, 0.3f, 0));
	moon->setAnimation(moonAnim);
	animations.push_back(moonAnim);
	objects.push_back(moon);

	//Objects:
	Mesh* floorMesh = new Mesh(verts, ind, tex);
	meshes.push_back(floorMesh);
	objects.push_back(new SceneObject(floorMesh, shaderProgram));

	sun = new Sun(lightShader);
	sun->setLight(sunLight);
	Animation* sunAnimation = new GoAround(100.0f, 0.00005f, glm::vec3(0, 0.3f, 0));
	animations.push_back(sunAnimation);
	sun->setAnimation(sunAnimation);
	objects.push_back(sun);

	//Stars:
	stars = new Stars(750);
}


void Scene::preShadowRenderPassInit()
{
	postprocessUnit.preShadowPassInit();
	glEnable(GL_DEPTH_TEST);
	sun->updateLightCamera(*planet);
}

void Scene::preGeometryRenderPassInit()
{
	postprocessUnit.preGeometryRenderPassInit(backgroundColor);
	glEnable(GL_DEPTH_TEST);
	camera->updateMatrix();
}



Scene* Scene::getInstance()
{
    if (instance == nullptr) {
        instance = new Scene();
    }
    return instance;
}

void Scene::destroyInstance()
{
    if (instance != nullptr) {
        delete instance;
        instance = nullptr;
    }
}


void Scene::init()
{
	postprocessUnit.init();
	initCamera();
	initMeshesShadersObjects();
	stars->exportStars(postprocessUnit.getShader());
}

void Scene::destroy()
{
	if (camera != nullptr) {
		delete camera;
		camera = nullptr;
	}

	if (stars != nullptr) {
		delete stars;
		stars = nullptr;
	}

	for (auto obj : objects) {
		delete obj;
	}
	objects.clear();

	for (auto lg : lights) {
		delete lg;
	}
	lights.clear();

	for (auto me : meshes) {
		delete me;
	}
	meshes.clear();

	for (auto sh : objectShaders) {
		sh->Delete();
		delete sh;
	}
	objectShaders.clear();

	for (auto anim : animations) {
		delete anim;
	}
	animations.clear();
}

//-----------------------------------------------------------------------------

void Scene::control(float dt)
{
    ControlActionManager::getInstance()->executeQueue(this, dt);
	
	for (auto obj : objects) {
		obj->control(dt);
	}
}

void Scene::animate(float dt)
{
	if (!pause) {
		for (auto obj : objects) {
			obj->animate(dt);
		}
		if (gravitation) {
			camera->prefUp = camera->prefUp * 0.95f + glm::normalize(camera->Position - planet->getPosition()) * 0.05f;
			if (glm::length(camera->Position - planet->getPosition()) > planet->getRadius() + 0.8f) {
				camera->Position = camera->Position + glm::normalize(planet->getPosition() - camera->Position) * dt * 0.001f;
			}
			else if (glm::length(camera->Position - planet->getPosition()) < planet->getRadius() + 0.75f) {
				camera->Position = planet->getPosition() - glm::normalize(planet->getPosition() - camera->Position) * (planet->getRadius() + 0.8f);
			}
		}
	}
}

void Scene::draw()
{
	preShadowRenderPassInit();
	for (auto obj : objects) {
		if (obj != sun) {
			obj->draw(*sun->getLightCamera());
		}
	}
	preGeometryRenderPassInit();
	for (auto lg : lights) {
		for (auto sh : objectShaders) {
			lg->exportData(sh);
		}
	}
	for (auto obj : objects) {
		obj->draw(*camera);
	}
	postprocessUnit.renderToScreen(*camera, *sun->getLightCamera(), *planet, *sun);
}

void Scene::togglePause()
{
	pause = !pause;
}

void Scene::toggleGravitation()
{
	gravitation = !gravitation;
}

Planet* Scene::getPlanet()
{
	return planet;
}

PostprocessUnit* Scene::getPostprocessUnit()
{
	return &postprocessUnit;
}

Camera* Scene::getCamera() {
    return camera;
}
