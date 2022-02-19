#pragma once
#include <glm/glm.hpp>


class ISceneObject
{
public:
	virtual void setPosition(glm::vec3 pos) = 0;
};

