#ifndef TEXTURE2D_CLASS_H
#define TEXTURE2D_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>

#include"shaderClass.h"

class Texture2D
{
	std::vector<glm::vec4> bytes;
	glm::ivec2 dimensions;
	glm::vec4 nullVector;

public:
	friend class FBO;

	GLuint ID;
	GLuint unit;
	const char* type;

	Texture2D(const char* image, GLuint unit, GLenum format, GLenum pixelType);

	Texture2D(std::vector<glm::vec4> bytes, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

	Texture2D(GLint internalformat, glm::ivec2 dimensions, GLuint unit, GLenum format, GLenum pixelType);

	~Texture2D() {
		this->Delete();
	}

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	const glm::ivec2 getDimensions() {
		return dimensions;
	}

	const std::vector<glm::vec4>& getBytes() {
		return bytes;
	}

	glm::vec4& operator()(glm::ivec2 position) {
		if (position.x >= dimensions.x || position.x < 0
			|| position.y >= dimensions.y || position.y < 0) {
			return nullVector;
		}
		glm::vec4 v = bytes[position.y * dimensions.x + position.x];
		return v;
	}

	glm::vec4& operator()(glm::vec2 normalisedPosition) {
		return operator()(glm::ivec2(
			normalisedPosition.x * (dimensions.x - 1),
			normalisedPosition.y * (dimensions.y - 1)));
	}
};
#endif