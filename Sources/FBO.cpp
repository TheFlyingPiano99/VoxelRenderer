#include "FBO.h"

FBO::FBO()
{
	glGenFramebuffers(1, &ID);
}

void FBO::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, ID);
}

void FBO::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::Delete()
{
	glDeleteFramebuffers(1, &ID);
}

void FBO::LinkTexture(GLenum attachment, Texture2D& texture, GLint level)
{
	Bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.ID, level);
}

void FBO::LinkRBO(GLenum attachment, RBO& rbo)
{
	Bind();
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, rbo.ID);
}
