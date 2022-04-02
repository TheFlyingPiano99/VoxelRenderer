#pragma once
#include<glad/glad.h>
#include "Texture2D.h"
#include "RBO.h"

class FBO
{
public:
	GLuint ID;

	FBO();

	// Binds the VAO
	void Bind();
	// Unbinds the VAO
	void Unbind();
	// Deletes the VAO
	void Delete();

	void LinkTexture(GLenum attachment, Texture2D& texture, GLint level);

	void LinkRBO(GLenum attachment, RBO& rbo);
};

