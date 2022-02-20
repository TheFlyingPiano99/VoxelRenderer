#include "PostprocessUnit.h"
#include "GlobalInclude.h"
#include "AssetManager.h"


void PostprocessUnit::exportData()
{
	glUniform1f(glGetUniformLocation(shader->ID, "gamma"), gamma);
	glUniform1f(glGetUniformLocation(shader->ID, "exposure"), exposure);
}

void PostprocessUnit::initColorFBO()
{
	// Generating Framebuffer:
	glGenFramebuffers(1, &colorFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);

	// Generating Color Texture for framebuffer:
	glGenTextures(2, framebufferColorTextures);
	for (int i = 0; i < 2; i++) {
		glBindTexture(GL_TEXTURE_2D, framebufferColorTextures[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, framebufferColorTextures[i], 0);
	}
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);

	// Generating Depth and Stencil Texture:
	glGenTextures(1, &framebufferDepthStencilTexture);
	glBindTexture(GL_TEXTURE_2D, framebufferDepthStencilTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, windowWidth, windowHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, framebufferDepthStencilTexture, 0);

	// Check for errors:
	auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer error: " << fboStatus << std::endl;

}

void PostprocessUnit::initShadowFBO()
{
	//Shadow Depth Map:
	glGenFramebuffers(1, &shadowDepthMapFBO);
	glGenTextures(1, &shadowDepthTexture);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
		SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void PostprocessUnit::init() {
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
	shader = new Shader(
		AssetManager::getInstance()->getShaderFolderPath().append("quad.vert").c_str(),
		AssetManager::getInstance()->getShaderFolderPath().append("postprocess.frag").c_str()
	);
	shader->Activate();

	// Generating Vertex array object and vertex buffer object for the quad:
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	initColorFBO();
	initShadowFBO();
}

void PostprocessUnit::preShadowPassInit()
{
	glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowDepthMapFBO);
	glClear(GL_DEPTH_BUFFER_BIT);

}

void PostprocessUnit::preGeometryRenderPassInit(const glm::vec4& backgroundColor)
{
	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, colorFBO);
	glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, backgroundColor.w);
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glUniform1i(glGetUniformLocation(shader->ID, "windowWidth"), windowWidth);
	glUniform1i(glGetUniformLocation(shader->ID, "windowHeight"), windowHeight);
}

void PostprocessUnit::renderToScreen(Camera& camera, Camera& lightCamera, Planet& planet, Sun& sun)
{
	shader->Activate();

	exportData();
	camera.exportData(*shader);
	lightCamera.exportPostprocessDataAsLightCamera(*shader);
	planet.exportAtmosphere(*shader);
	sun.exportData(*shader);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glBindVertexArray(rectVAO);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0 + 0);
	glBindTexture(GL_TEXTURE_2D, framebufferColorTextures[0]);

	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, framebufferColorTextures[1]);

	glActiveTexture(GL_TEXTURE0 + 2);
	glBindTexture(GL_TEXTURE_2D, framebufferDepthStencilTexture);

	glActiveTexture(GL_TEXTURE0 + 3);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);

	glDrawArrays(GL_TRIANGLES, 0, 6);
}


Shader* PostprocessUnit::getShader()
{
	return shader;
}


PostprocessUnit::~PostprocessUnit()
{
	glDeleteFramebuffers(1, &colorFBO);
	glBindVertexArray(rectVAO);
	glDeleteBuffers(1, &rectVBO);
	glDeleteVertexArrays(1, &rectVAO);
}

float* PostprocessUnit::getGamma()
{
	return &gamma;
}

float* PostprocessUnit::getExposure()
{
	return &exposure;
}
