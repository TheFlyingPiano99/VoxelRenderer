#pragma once
#include<glad/glad.h>
#include "Texture2D.h"
#include "RBO.h"

class FBO
{
public:
	GLuint ID;
	glm::ivec4 viewport;

	FBO();

	void Bind();
	void Unbind();
	void Delete();

	void LinkTexture(GLenum attachment, Texture2D& texture, GLint level);

	void LinkRBO(GLenum attachment, RBO& rbo);

	static void BindDefault();
};

