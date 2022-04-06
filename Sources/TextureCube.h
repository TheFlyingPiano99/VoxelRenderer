#pragma once
#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>

#include"shaderClass.h"

class TextureCube
{
	glm::ivec2 dimensions;
	glm::vec4 nullVector;

public:
	friend class FBO;

	GLuint ID;
	GLuint unit;
	const char* type;

	TextureCube(std::vector<std::string>& images, GLuint unit);

	~TextureCube() {
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

};
