#include "Scene.h"
#include "GlobalInclude.h"
#include "ControlActionManager.h"
#include "AssetManager.h"

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

void Scene::initQuad()
{
	float quadVertices[] =
	{
		//Coord	//texCoords
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f, -1.0f,  0.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f,

		1.0f,  1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,  0.0f,
	   -1.0f,  1.0f,  0.0f,  1.0f
	};

	quadVAO.Bind();
	unsigned int quadVBO;
	glGenBuffers(1, &quadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	quadVAO.Unbind();
}

void Scene::initCamera()
{
	camera = new Camera(contextWidth, contextHeight, glm::vec3(100.0f, 0.0f, 100.0f), glm::vec3(0, 0, 0));
}

void Scene::initMeshesShadersObjects()
{
	Shader* voxelShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("voxel.frag").c_str()
	);
	Shader* boundingShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("bounding.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("bounding.frag").c_str()
	);
	Shader* transferShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("transfer.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("transfer.frag").c_str()
	);
	shaders.push_back(voxelShader);
	shaders.push_back(boundingShader);
	shaders.push_back(transferShader);
	voxels = new VoxelData(voxelShader, boundingShader, transferShader, &quadVAO, "D:/VisualCpp/VoxelRenderer/Resources/Volumetric/cthead-8bit/", contextWidth, contextHeight);
}


void Scene::preShadowRenderPassInit()
{
	postprocessUnit.preShadowPassInit();
	glEnable(GL_DEPTH_TEST);
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
        instance = new Scene(WINDOW_WIDTH, WINDOW_HEIGHT);
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
	initQuad();
	initCamera();
	initMeshesShadersObjects();
}

void Scene::destroy()
{
	if (camera != nullptr) {
		delete camera;
		camera = nullptr;
	}

	for (auto lg : lights) {
		delete lg;
	}
	lights.clear();

	for (auto sh : shaders) {
		sh->Delete();
		delete sh;
	}
	shaders.clear();
	quadVAO.Delete();
}

//-----------------------------------------------------------------------------

void Scene::control(float dt)
{
    ControlActionManager::getInstance()->executeQueue(this, dt);

	voxels->optimize(dt, pause, cameraLastActive);
	cameraLastActive += dt;
}

void Scene::animate(float dt)
{
	if (!pause) {
		voxels->animate(dt);
	}
}

void Scene::draw()
{
	//preGeometryRenderPassInit();
	camera->updateMatrix();
	voxels->draw(*camera);
	//postprocessUnit.renderToScreen(*camera, *sun->getLightCamera(), *planet, *sun);
}

void Scene::togglePause()
{
	pause = !pause;
}

void Scene::toggleGravitation()
{
	gravitation = !gravitation;
}

PostprocessUnit* Scene::getPostprocessUnit()
{
	return &postprocessUnit;
}

Camera* Scene::getCamera() {
	cameraLastActive = 0.0f;
    return camera;
}

VoxelData* Scene::getVoxelData()
{
	return voxels;
}
