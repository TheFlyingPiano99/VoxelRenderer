#include "FBO.h"
#include "GlobalInclude.h"

FBO::FBO()
{
	glGenFramebuffers(1, &ID);
}

void FBO::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
	glViewport(viewport.x, viewport.y, viewport.z, viewport.w);
}

void FBO::Unbind()
{
	BindDefault();
}

void FBO::Delete()
{
	glDeleteFramebuffers(1, &ID);
}

void FBO::LinkTexture(GLenum attachment, Texture2D& texture, GLint level)
{
	glm::ivec2 dim = texture.getDimensions();
	viewport = glm::vec4(0, 0, dim.x, dim.y);
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.ID, level);
}


void FBO::LinkRBO(GLenum attachment, RBO& rbo)
{
	Bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo.ID);
}



void FBO::BindDefault()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, GlobalVariables::windowWidth, GlobalVariables::windowHeight);
}
