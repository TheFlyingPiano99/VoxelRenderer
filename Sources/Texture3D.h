#ifndef TEXTURE3D_CLASS_H
#define TEXTURE3D_CLASS_H

#include<glad/glad.h>
#include<stb/stb_image.h>

#include"shaderClass.h"

class Texture3D
{
public:
	GLuint ID;
	GLuint unit;
	float shininess = 16.0f;	// Should be extracted to Material

	Texture3D(const char* image, GLuint slot, GLenum format, GLenum pixelType);

	// Assigns a texture unit to a texture
	void texUnit(Shader& shader, const char* uniform, GLuint unit);
	// Binds a texture
	void Bind();
	// Unbinds a texture
	void Unbind();
	// Deletes a texture
	void Delete();
};
#endif