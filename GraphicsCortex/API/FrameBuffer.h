#pragma once
#include "Config.h"

#include "GL\glew.h"

#include <string>
#include "Texture.h"
#include "ShaderCompiler.h"
#include "Graphic.h"

#include <type_traits>

class FrameBuffer {
public:
	enum
	{
		COLOR_TEXTURE = 0,
		DEPTH_TEXTURE,
		STENCIL_TEXTURE,
	};

	unsigned int id = 0;
	int width = 0, height = 0;
	int texture_slot = 9;
	Texture color_texture;
	Texture depth_stencil_texture;
	RenderBuffer depth_stencil_renderbuffer;
	bool readable_depth_stencil_buffer;

	Program_s program;

	FrameBuffer(int width, int height, int anti_alliasing = 0, bool readable_depth_stencil_buffer = false);
	~FrameBuffer();

	void release();

	void generate_framebuffer_object();
	bool is_initialized();

	void bind();
	void unbind();
	void render(unsigned int source_texture = FrameBuffer::COLOR_TEXTURE);
	Image save(bool vertical_flip = true);

	void load_program(Program_s program);

private:

	Graphic screen;
	
	bool _is_initialized = false;
	bool _is_program_loaded = false;
	int multisample = 0;
	bool screen_initialized = false;
};