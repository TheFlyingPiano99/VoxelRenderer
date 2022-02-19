#pragma once


#include "shaderClass.h"
#include<glm/glm.hpp>

class LightSource
{
protected:

	glm::vec3 ambient;
	glm::vec3 diffuse;
	glm::vec3 specular;

	LightSource(glm::vec3 _ambient, glm::vec3 _diffuse, glm::vec3 _specular) : ambient(_ambient), diffuse(_diffuse), specular(_specular) {
	}

public:

	/*
	* Export light source data onto shader
	*/
	virtual void exportData(Shader* shader) = 0;

	glm::vec3 getDiffuse() {
		return diffuse;
	}
};

class DirectionalLight : public LightSource {
	glm::vec3 direction;
public:
	DirectionalLight(glm::vec3 _direction, glm::vec3 _diffuse)
		: LightSource(glm::vec3(0.01f, 0.01f, 0.01f), _diffuse, glm::vec3(0.3f, 0.3f, 0.3f)),
	direction(_direction)
	{
	}

	virtual void exportData(Shader* shader) override;

};

class PointLight : public LightSource {
	unsigned int index;
	glm::vec3 position;

	float constant;
	float linear;
	float quadratic;

public:
	PointLight(unsigned int _index, glm::vec3 _pos, glm::vec3 _diffuse)
		: LightSource(glm::vec3(0.2f, 0.2f, 0.2f), _diffuse, glm::vec3(0.5f, 0.5f, 0.5f)),
		index(_index), position(_pos), constant(1.0f), linear(0.7f), quadratic(3.0f)
	{
	}

	void exportData(Shader* shader) override;

	void setPosition(glm::vec3 _pos) {
		position = _pos;
	}

	void setAttenuation(float _quadratic, float _linear, float _constant) {
		quadratic = _quadratic;
		linear = _linear;
		constant = _constant;
	}

};

