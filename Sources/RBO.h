#pragma once
#include <glad/glad.h>
class RBO
{
public:
	GLuint ID;

	RBO(GLenum internalformat, GLsizei width, GLsizei height);

	void Bind();

	void Delete();
};

