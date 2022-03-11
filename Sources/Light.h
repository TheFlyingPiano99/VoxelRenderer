#pragma once
#include<glm/glm.hpp>

struct Light {
	glm::vec3 position;
	glm::vec3 intensity;
	glm::mat4 viewProjMatrix;
};