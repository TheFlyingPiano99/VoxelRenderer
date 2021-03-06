#pragma once
#include <glad/glad.h>
#include <vector>

class RBO
{
public:
	friend class FBO;
	GLuint ID;

	RBO(GLenum internalformat, GLsizei width, GLsizei height);

	void Bind();

	void Delete();
};

