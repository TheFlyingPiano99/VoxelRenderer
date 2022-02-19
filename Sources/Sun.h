#pragma once
#include "SceneObject.h"
#include "Planet.h"
#include "Camera.h"

class Sun : public SceneObject
{
	Camera* lightCamera;
public:

	Sun(Shader* _shader);

	~Sun();

	void exportData(Shader& shader);
	void updateLightCamera(Planet& planet);
	Camera* getLightCamera();
};

