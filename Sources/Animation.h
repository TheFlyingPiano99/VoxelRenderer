#pragma once

#include "ISceneObject.h"

class Animation
{
protected:
	float t = 0.0f;

public:
	virtual void perform(ISceneObject* obj, float dt) = 0;
	void reset();
};


class GoAround : public Animation {
	float angularVelocity = 1.0;
	float radius;
	glm::vec3 center;

public:
	GoAround(float _r, float _angVel, glm::vec3 _center = glm::vec3(0.0f)) : radius(_r), angularVelocity(_angVel), center(_center) {
	}

	void perform(ISceneObject* obj, float dt) override {
		t += dt;
		obj->setPosition(center + glm::vec3(radius * sin(angularVelocity * t), 0, radius * cos(angularVelocity * t)));
	}
};

