#ifndef TEXTURE3D_CLASS_H
#define TEXTURE3D_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>
#include<glm/glm.hpp>
#include <vector>

#include"shaderClass.h"

struct Dimensions {
	int width, height, depth, bytesPerVoxel;
	float widthScale, heightScale, depthScale = 1.0f;
};

class Texture3D
{
public:

	GLuint ID;
	GLuint unit;
	float shininess = 16.0f;	// Should be extracted to Material
	std::vector<char> bytes;
	const Dimensions dimensions;
	int maxValue;
	Texture3D(const char* directory, const Dimensions dimensions, GLuint slot, GLenum format);
	~Texture3D();

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	const Dimensions& getDimensions() {
		return dimensions;
	}

	const float operator()(glm::ivec3 position);

	const glm::vec4 resampleGradientAndDensity(glm::ivec3 position);

};
#endif