#pragma once

#include<glm/glm.hpp>
#include "shaderClass.h"
#include "Texture2D.h"
#include "VAO.h"
#include "Texture3D.h"


class TransferFunction
{
	Shader* shader;
	glm::mat4 modelMatrix;
	glm::mat4 invModelMatrix;
	Texture2D* texture = nullptr;
	VAO* quadVAO;
	float displayExposure = 1.0f;
	float displayGamma = 1.0f;
	glm::vec4 nullVector = glm::vec4(0.0f);

public:
	TransferFunction(Shader* shader, VAO* quad);

	~TransferFunction() {
		if (texture != nullptr) {
			delete texture;
		}
	}

	void crop(glm::vec2 min, glm::vec2 max);
	void floodFill(glm::vec2 startPos, glm::vec4 color, float threshold);
	void blur(int kernelSize);
	void normalize();
	void draw();
	void Bind();
	void Unbind();

	const glm::ivec2 getDimensions() {
		if (texture == nullptr)
			return glm::ivec2(0);
		return texture->getDimensions();
	}

	const glm::mat4 getModelMatrix() {
		return modelMatrix;
	}

	const glm::mat4 getInvModelMatrix() {
		return invModelMatrix;
	}

	void defaultTransferFunction(glm::ivec2 dimensions);
	void spatialTransferFunction(glm::ivec2 dimensions, Texture3D& voxels);

	glm::vec4& operator()(glm::ivec2 position) {
		if (texture == nullptr)
			return nullVector;
		return texture->operator()(position);
	}

	glm::vec4& operator()(glm::vec2 normalisedPosition) {
		if (texture == nullptr)
			return nullVector;
		return texture->operator()(normalisedPosition);
	}

	void operator=(TransferFunction& transferFunction);

	void setCamSpacePosition(glm::vec2 camPos);

	float& getExposure() {
		return displayExposure;
	}

	float& getGamma() {
		return displayGamma;
	}

};

