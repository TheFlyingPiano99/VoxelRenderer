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

void Scene::initQuadFBO()
{
	if (quadColorTexture != nullptr) {
		delete quadColorTexture;
		delete quadDepthTexture;
	}
	quadColorTexture = new Texture2D(GL_RGBA, glm::ivec2(contextWidth, contextHeight), 0, GL_RGBA, GL_FLOAT);
	quadDepthTexture = new Texture2D(GL_DEPTH_COMPONENT, glm::ivec2(contextWidth, contextHeight), 6, GL_DEPTH_COMPONENT, GL_FLOAT);
	quadFBO.LinkTexture(GL_COLOR_ATTACHMENT0, *quadColorTexture, 0);
	quadFBO.LinkTexture(GL_DEPTH_ATTACHMENT, *quadDepthTexture, 0);
	RBO stencilRBO(GL_STENCIL_COMPONENTS, contextWidth, contextHeight);
	//quadFBO.LinkRBO(GL_STENCIL_ATTACHMENT, stencilRBO);
}

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

void Scene::initBasePlate()
{
	Shader* defaultIncrementalShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("default.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("default.frag").c_str()
	);
	shaders.push_back(defaultIncrementalShader);

	std::vector<Vertex> infinitePlaneVertices;
	std::vector<GLuint> infinitePlaneIndices;
	std::vector<Texture2D*> infinitePlaneTextures;

	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
	Vertex v1;
	v1.color = color;
	v1.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v1.position = glm::vec4(-1.0f, 0.0f, -1.0f, 1.0f);
	v1.texUV = glm::vec2(0.0f, 1.0f);

	Vertex v2;
	v2.color = color;
	v2.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v2.position = glm::vec4(1.0f, 0.0f, -1.0f, 1.0f);
	v2.texUV = glm::vec2(1.0f, 1.0f);

	Vertex v3;
	v3.color = color;
	v3.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v3.position = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);
	v3.texUV = glm::vec2(1.0f, 0.0f);

	Vertex v4;
	v4.color = color;
	v4.normal = glm::vec3(0.0f, 1.0f, 0.0f);
	v4.position = glm::vec4(-1.0f, 0.0f, 1.0f, 1.0f);
	v4.texUV = glm::vec2(0.0f, 0.0f);

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

	Texture2D* colorTexture = new Texture2D(AssetManager::getInstance()->getTextureFolderPath().append("planks.png").c_str(), "diffuse", 0, GL_RGBA, GL_UNSIGNED_BYTE);
	Texture2D* specularTexture = new Texture2D(AssetManager::getInstance()->getTextureFolderPath().append("planksSpec.png").c_str(), "specular", 1, GL_RED, GL_UNSIGNED_BYTE);
	colorSpecularTextures.push_back(colorTexture);
	infinitePlaneTextures.push_back(colorTexture);
	colorSpecularTextures.push_back(specularTexture);
	infinitePlaneTextures.push_back(specularTexture);

	SceneObject* plane = new SceneObject(new Mesh(infinitePlaneVertices, infinitePlaneIndices, infinitePlaneTextures),
		defaultIncrementalShader);
	plane->setScale(glm::vec3(300, 1, 300));
	sceneObjects.push_back(plane);


	plane = new SceneObject(new Mesh(infinitePlaneVertices, infinitePlaneIndices, infinitePlaneTextures),
		defaultIncrementalShader);
	plane->setScale(glm::vec3(300, 1, 300));
	plane->setEulerAngles(glm::vec3(3.14159265359f, 0.0f, 0.0f));
	sceneObjects.push_back(plane);
}

void Scene::initCameraAndLights()
{
	camera = new Camera(contextWidth, contextHeight, glm::vec3(500.0f, 400.0f, -500.0f), glm::vec3(0, 256 / 2.0f, 0));
	lights.push_back(Light());	// Headlight
	lights[0].powerDensity = glm::vec3(headLightPower);
	camera->moved = true;
	lights.push_back(Light());	// Directional light
	lights[1].position = glm::normalize(glm::vec4(-1.0f, 1.0f, -1.0f, 0.0f));
	lights[1].powerDensity = glm::vec3(0.9f, 0.9f, 0.9f);

	lights.push_back(Light());	// X light
	lights[2].position = glm::vec4(250.0f, 1.0f, 0.0f, 1.0f);
	lights[2].powerDensity = glm::vec3(100.0f, 50.0f, 0.0f);

	lights.push_back(Light());	// -X light
	lights[3].position = glm::vec4(-250.0f, 1.0f, 0.0f, 1.0f);
	lights[3].powerDensity = glm::vec3(100.0f, 50.0f, 0.0f);

	lights.push_back(Light());	// Z light
	lights[4].position = glm::vec4(0.0f, 1.0f, 250.0f, 1.0f);
	lights[4].powerDensity = glm::vec3(0.0f, 50.0f, 100.0f);

	lights.push_back(Light());	// -Z light
	lights[5].position = glm::vec4(0.0f, 1.0f, -250.0f, 1.0f);
	lights[5].powerDensity = glm::vec3(0.0f, 50.0f, 100.0f);
}

void Scene::initMeshesShadersObjects()
{

	Shader* voxelShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("voxel.frag").c_str()
	);
	Shader* voxelHalfAngleShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("voxelHalfAngle.frag").c_str()
	);
	Shader* quadShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("quad.frag").c_str()
	);
	Shader* quadDepthShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("quad-depth.frag").c_str()
	);
	Shader* boundingShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("bounding.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("bounding.frag").c_str()
	);
	Shader* flatColorBoundingShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("bounding.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("flatColor.frag").c_str()
	);
	Shader* transferShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("transfer.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("transfer.frag").c_str()
	);
	Shader* skyboxShader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("skybox.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("skybox.frag").c_str()
	);

	shaders.push_back(voxelShader);
	shaders.push_back(voxelHalfAngleShader);
	shaders.push_back(quadShader);
	shaders.push_back(quadDepthShader);
	shaders.push_back(boundingShader);
	shaders.push_back(flatColorBoundingShader);
	shaders.push_back(transferShader);
	shaders.push_back(skyboxShader);

	std::vector<std::string> images;
	images.push_back(AssetManager::getInstance()->getTextureFolderPath().append("right.jpg").c_str());
	images.push_back(AssetManager::getInstance()->getTextureFolderPath().append("left.jpg").c_str());
	images.push_back(AssetManager::getInstance()->getTextureFolderPath().append("top.jpg").c_str());
	images.push_back(AssetManager::getInstance()->getTextureFolderPath().append("bottom.jpg").c_str());
	images.push_back(AssetManager::getInstance()->getTextureFolderPath().append("front.jpg").c_str());
	images.push_back(AssetManager::getInstance()->getTextureFolderPath().append("back.jpg").c_str());
	skybox = new SkyBox(images, skyboxShader);

	this->quadShader = quadShader;
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
	voxels = new VoxelData(voxelShader, voxelHalfAngleShader, quadDepthShader, boundingShader, flatColorBoundingShader, transferShader, &quadVAO, paths[selection], contextWidth, contextHeight);
	voxels->loadFeatures();
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
	initQuadFBO();
	initQuad();
	initBasePlate();
	initCameraAndLights();
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
	sceneObjects.clear();
	for (auto texture : colorSpecularTextures) {
		delete texture;
	}
	colorSpecularTextures.clear();
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
	for (auto obj : sceneObjects) {
		obj->animate(dt);
	}
	voxels->animate(dt);

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

	quadFBO.Bind();
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	glClearDepth(1);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glStencilMask(0x00);


	skybox->draw(*camera);
	for (auto obj : sceneObjects) {
		obj->update();
		obj->draw(*camera, lights);
	}
	if (cameraMoved || voxels->popChanged()) {
		partToDraw = 0;
		voxels->drawBoundingGeometry(*camera, lights);
		voxels->resetOpacity();
	}
	if (partToDraw >= 0) {
		voxels->drawHalfAngleLayer(*camera, *quadDepthTexture, lights[0], *skybox, partToDraw, noOfPartsToDraw);
		partToDraw++;
		if (partToDraw == noOfPartsToDraw) {
			partToDraw = -1;
		}
	}

	quadFBO.Bind();
	if (partToDraw >= 0) {
		voxels->drawBoundingGeometryOnScreen(*camera, (1.0f - std::powf(partToDraw / (float)noOfPartsToDraw, 0.2f)) * 0.3f);
	}

	voxels->drawQuad(*quadDepthTexture);
	voxels->drawTransferFunction();

	FBO::BindDefault();
	quadShader->Activate();
	quadVAO.Bind();
	quadColorTexture->Bind();
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
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
	if (voxels != nullptr) {
		voxels->onContextResize(contextWidth, contextHeight);
	}
	initQuadFBO();
	camera->moved = true;
}

void Scene::serialize()
{
	if (nullptr != voxels) {
		voxels->saveFeatures();
	}
}
