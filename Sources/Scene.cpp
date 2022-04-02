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
	Vertex{glm::vec4(-1.0f, 0.0f,  1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
	Vertex{glm::vec4(-1.0f, 0.0f, -1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
	Vertex{glm::vec4(1.0f, 0.0f, -1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
	Vertex{glm::vec4(1.0f, 0.0f,  1.0f, 1.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}
};

// Indices for vertices order
GLuint indices[] =
{
	0, 1, 2,
	0, 2, 3
};

Vertex lightVertices[] =
{ //     COORDINATES     //
	Vertex{glm::vec4(-0.1f, -0.1f,  0.1f, 1.0f)},
	Vertex{glm::vec4(-0.1f, -0.1f, -0.1f, 1.0f)},
	Vertex{glm::vec4(0.1f, -0.1f, -0.1f, 1.0f)},
	Vertex{glm::vec4(0.1f, -0.1f,  0.1f, 1.0f)},
	Vertex{glm::vec4(-0.1f,  0.1f,  0.1f, 1.0f)},
	Vertex{glm::vec4(-0.1f,  0.1f, -0.1f, 1.0f)},
	Vertex{glm::vec4(0.1f,  0.1f, -0.1f, 1.0f)},
	Vertex{glm::vec4(0.1f,  0.1f,  0.1f, 1.0f)}
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

void Scene::initInfinitePlane()
{
	Shader* defaultIncrementalShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("default.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("default.frag").c_str()
	);
	shaders.push_back(defaultIncrementalShader);

	std::vector<Vertex> infinitePlaneVertices;
	std::vector<GLuint> infinitePlaneIndices;
	std::vector<Texture2D> infinitePlaneTextures;

	glm::vec3 color = glm::vec3(0.0f, 0.5f, 0.2f);
	Vertex v1;
	v1.color = color;
	v1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v1.position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	v1.texUV = glm::vec2(0.5f, 0.5f);

	Vertex v2;
	v2.color = color;
	v2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v2.position = glm::vec4(-1.0f, 0.0f, -1.0f, 0.0f);
	v2.texUV = glm::vec2(0.0f, 1.0f);

	Vertex v3;
	v3.color = color;
	v3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v3.position = glm::vec4(1.0f, 0.0f, -1.0f, 0.0f);
	v3.texUV = glm::vec2(1.0f, 1.0f);

	Vertex v4;
	v4.color = color;
	v4.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v4.position = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
	v4.texUV = glm::vec2(0.5f, 0.0f);

	infinitePlaneVertices.push_back(v1);
	infinitePlaneVertices.push_back(v2);
	infinitePlaneVertices.push_back(v3);
	infinitePlaneVertices.push_back(v4);
	infinitePlaneIndices.push_back(2);
	infinitePlaneIndices.push_back(1);
	infinitePlaneIndices.push_back(0);

	infinitePlaneIndices.push_back(3);
	infinitePlaneIndices.push_back(2);
	infinitePlaneIndices.push_back(0);

	infinitePlaneIndices.push_back(1);
	infinitePlaneIndices.push_back(3);
	infinitePlaneIndices.push_back(0);

	SceneObject* infinitePlane = new SceneObject(new Mesh(infinitePlaneVertices, infinitePlaneIndices, infinitePlaneTextures),
		defaultIncrementalShader);
	sceneObjects.push_back(infinitePlane);

}

void Scene::initCamera()
{
	camera = new Camera(contextWidth, contextHeight, glm::vec3(500.0f, 400.0f, 500.0f), glm::vec3(0, 300, 0));
	lights.push_back(Light());	// Headlight
	lights[0].powerDensity = glm::vec3(headLightPower);
	camera->moved = true;
	//lights.push_back(Light());	// Static light
	//lights[1].position = glm::vec4(0.0f, 300.0f, 300.0f, 1.0f);
	//lights[1].powerDensity = glm::vec3(10000.0f, 10000.0f, 10000.0f);

}

void Scene::initMeshesShadersObjects()
{

	Shader* voxelShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("voxel.frag").c_str()
	);
	quadShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("quad.frag").c_str()
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
	shaders.push_back(quadShader);
	shaders.push_back(boundingShader);
	shaders.push_back(transferShader);


	const char* paths[3] = {
		"D:/VisualCpp/VoxelRenderer/Resources/Volumetric/cthead-8bit/",
		"D:/VisualCpp/VoxelRenderer/Resources/Volumetric/mrbrain-8bit/",
		"D:/VisualCpp/VoxelRenderer/Resources/Volumetric/bunny/",
	};
	std::cout << "Select data source: [0, 1, 2]" << std::endl;
	int selection;
	std::cin >> selection;
	if (selection > 2 || selection < 0) {
		selection = 0;
	}
	voxels = new VoxelData(voxelShader, boundingShader, transferShader, &quadVAO, paths[selection], contextWidth, contextHeight);
}

void Scene::initQuadFBO()
{
	if (quadFBO > 0) {	// Not the first call
		glDeleteFramebuffers(1, &quadFBO);
		glDeleteTextures(1, &quadTexture);
	}

	glGenFramebuffers(1, &quadFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, quadFBO);

	glGenTextures(1, &quadTexture);
	glBindTexture(GL_TEXTURE_2D, quadTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, contextWidth, contextHeight, 0, GL_RGBA, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, quadTexture, 0);
}


Scene* Scene::getInstance()
{
    if (instance == nullptr) {
        instance = new Scene(GlobalVariables::windowWidth, GlobalVariables::windowHeight);
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


void Scene::init(int contextWidth, int contextHeight)
{
	this->contextWidth = contextWidth;
	this->contextHeight = contextHeight;
	initQuad();
	//initInfinitePlane();
	initQuadFBO();
	initCamera();
	initMeshesShadersObjects();
}

void Scene::destroy()
{
	if (camera != nullptr) {
		delete camera;
		camera = nullptr;
	}

	lights.clear();

	for (auto sh : shaders) {
		sh->Delete();
		delete sh;
	}
	shaders.clear();
	quadVAO.Delete();
	if (voxels != nullptr) {
		delete voxels;
		voxels = nullptr;
	}

	for (auto obj : sceneObjects) {
		delete obj;
	}
}

//-----------------------------------------------------------------------------

void Scene::control(float dt)
{
    ControlActionManager::getInstance()->executeQueue(this, dt);

	for (auto obj : sceneObjects) {
		obj->control(dt);
	}

	voxels->control(dt, pause, cameraLastActive);
	cameraLastActive += dt;
}

void Scene::animate(float dt)
{
	if (!pause) {
		for (auto obj : sceneObjects) {
			obj->animate(dt);
		}
		voxels->animate(dt);
	}

	glm::vec3 dir = glm::normalize(camera->center - camera->eye);
	glm::vec3 right = glm::cross(dir, camera->prefUp);
	glm::vec3 up = glm::cross(right, dir);
	glm::vec3 p = voxels->getPosition() - dir * 200.0f + up * 200.0f +  right * 200.0f;
	lights[0].position = glm::vec4(p.x, p.y, p.z, 1.0f);
	lights[0].powerDensity = glm::vec3(headLightPower);
	static float prevPower;
	if (prevPower != headLightPower) {
		camera->moved = true;
	}
	prevPower = headLightPower;
}

void Scene::draw()
{

	bool cameraMoved = camera->update();

	if (cameraMoved || voxels->popChanged()) {
		partToDraw = 0;
	}

	if (partToDraw >= 0) {
		glBindFramebuffer(GL_FRAMEBUFFER, quadFBO);
		glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto obj : sceneObjects) {
			obj->draw(*camera, lights);
		}

		voxels->draw(*camera, lights, quadFBO, glm::vec2(1.0f, 1.0f), glm::vec2(0.0f, 0.0f), (partToDraw + 1) / (float)noOfPartsToDraw);
		partToDraw++;
		
		if (partToDraw == noOfPartsToDraw) {
			partToDraw = -1;
		}
	}

	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	quadShader->Activate();
	quadVAO.Bind();
	glUniform2f(glGetUniformLocation(quadShader->ID, "scale"), 1.0f, 1.0f);
	glUniform2f(glGetUniformLocation(quadShader->ID, "offset"), 0.0f, 0.0f);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, quadTexture);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
}

void Scene::togglePause()
{
	pause = !pause;
}

void Scene::toggleGravitation()
{
	gravitation = !gravitation;
}

Camera* Scene::getCamera() {
	cameraLastActive = 0.0f;
    return camera;
}

VoxelData* Scene::getVoxelData()
{
	return voxels;
}

void Scene::onContextResize(int contextWidth, int contextHeight)
{
	this->contextWidth = contextWidth;
	this->contextHeight = contextHeight;
	if (camera != nullptr) {
		camera->width = contextWidth;
		camera->height = contextHeight;
	}
	initQuadFBO();
	if (voxels != nullptr) {
		voxels->onContextResize(contextWidth, contextHeight);
	}
	camera->moved = true;
}
