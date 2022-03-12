#ifndef TEXTURE1D_CLASS_H
#define TEXTURE1D_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"

class Texture1D
{
public:
	GLuint ID;
	const char* type;
	GLuint unit;
	float shininess = 16.0f;	// Should be extracted to Material
	unsigned char* bytes = nullptr;
	unsigned int width = 0;

	Texture1D(unsigned char* bytes, int width, GLuint slot, GLenum format, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();

	const unsigned char* getBytes() {
		return bytes;
	}

	const unsigned int getWidth() {
		return width;
	}
};
#endif