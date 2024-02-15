#include "Renderbuffer.h"
#include "Debuger.h"
#include <iostream>

Renderbuffer2::Renderbuffer2(int width, int height, ColorTextureFormat internal_format, int multisample):
	width(width), height(height), is_color_texture(true), color_texture_format(internal_format), multisample_amount(multisample)
{
	_generate_texture();
}

Renderbuffer2::Renderbuffer2(int width, int height, DepthStencilTextureFormat internal_format, int multisample) :
	width(width), height(height), is_color_texture(false), depth_stencil_texture_format(internal_format), multisample_amount(multisample)
{
	_generate_texture();
}

Renderbuffer2::~Renderbuffer2()
{
	release();
}

void Renderbuffer2::release()
{
	if (_texture_generated)
		GLCall(glDeleteTextures(1, &id));
	_texture_generated = false;
	_texture_allocated = false;
}

void Renderbuffer2::bind()
{
	if (!_texture_generated) {
		std::cout << "[OpenGL Error] released Renderbuffer tried to bind()" << std::endl;
		ASSERT(false);
	}
	GLCall(glBindTexture(target, id));
}

void Renderbuffer2::unbind()
{
	GLCall(glBindTexture(target, 0));
}

int Renderbuffer2::query_width()
{
	int texture_width;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_WIDTH, &texture_width));
	return texture_width;
}

int Renderbuffer2::query_height()
{
	int texture_height;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_HEIGHT, &texture_height));
	return texture_height;
}

int Renderbuffer2::query_internal_format()
{
	int internal_format;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_INTERNAL_FORMAT, &internal_format));
	return internal_format;
}

int Renderbuffer2::query_samples()
{
	int texture_samples;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_SAMPLES, &texture_samples));
	return texture_samples;
}

int Renderbuffer2::query_red_size()
{
	int texture_red_size;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_RED_SIZE, &texture_red_size));
	return texture_red_size;
}

int Renderbuffer2::query_green_size()
{
	int texture_green_size;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_GREEN_SIZE, &texture_green_size));
	return texture_green_size;
}

int Renderbuffer2::query_blue_size()
{
	int texture_blue_size;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_BLUE_SIZE, &texture_blue_size));
	return texture_blue_size;
}

int Renderbuffer2::query_alpha_size()
{
	int texture_alpha_size;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_ALPHA_SIZE, &texture_alpha_size));
	return texture_alpha_size;
}

int Renderbuffer2::query_depth_size()
{
	int texture_depth_size;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_DEPTH_SIZE, &texture_depth_size));
	return texture_depth_size;
}

int Renderbuffer2::query_stencil_size()
{
	int texture_stencil_size;
	GLCall(glGetNamedRenderbufferParameteriv(id, GL_RENDERBUFFER_STENCIL_SIZE, &texture_stencil_size));
	return texture_stencil_size;
}

void Renderbuffer2::_generate_texture()
{
	if (_texture_generated) return;
	if (multisample_amount != 0) target = GL_RENDERBUFFER_SAMPLES;
	GLCall(glCreateRenderbuffers(1, &id));
	_texture_generated = true;
}

void Renderbuffer2::_allocate_texture()
{
	if (!_texture_generated) return;
	if (_texture_allocated) return;

	unsigned int gl_internal_format = is_color_texture ? ColorTextureFormat_to_OpenGL(color_texture_format) : DepthStencilTextureFormat_to_OpenGL(depth_stencil_texture_format);

	if (multisample_amount == 0) {
		GLCall(glNamedRenderbufferStorage(id, gl_internal_format, width, height));
	}
	else {
		GLCall(glNamedRenderbufferStorageMultisample(id, multisample_amount, gl_internal_format, width, height));
	}

	_texture_allocated = true;
}

