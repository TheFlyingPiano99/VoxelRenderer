#pragma once

#include<glm/glm.hpp>
#include "shaderClass.h"
#include "Camera.h"
#include "Planet.h"
#include "Sun.h"



class PostprocessUnit
{
	unsigned int colorFBO;
	unsigned int RBO;
	Shader* shader;
	unsigned int rectVAO, rectVBO;
	unsigned int framebufferColorTextures[2];
	unsigned int framebufferDepthStencilTexture;

	unsigned int shadowDepthMapFBO;
	unsigned int shadowDepthTexture;

	float gamma = 1.1f;
	float exposure = 0.98f;


	void exportData();

	void initColorFBO();
	void initShadowFBO();

public:

	void init();
	void preShadowPassInit();
	void preGeometryRenderPassInit(const glm::vec4& backgroundColor);
	void renderToScreen(Camera& camera, Camera& lightCamera, Planet& planet, Sun& sun);
	Shader* getShader();
	~PostprocessUnit();

	float* getGamma();
	float* getExposure();
};

