#include "Text.h" 
#include "Default_Programs.h"

Program_s Text::_default_text_renderer;
bool Text::_default_renderer_initialized = false;

Text::Text(Font_s font, const std::string& text) :
	_font(font)
{
	set_text(text);
}

Text::Text(Font_s font, Program_s custom_renderer, const std::string& text) :
	_font(font)
{
	set_text(text);
	graphic->load_program(custom_renderer);
	_custom_renderer_loaded = true;
}

void Text::set_text(const std::string& text){
	_graphic_needs_update = true;
	_text = text;
}

std::string Text::get_text(){
	return _text;
}

void Text::set_scale(float scale){
	_graphic_needs_update = true;
	_scale = scale;
}

float Text::get_scale(){
	return _scale;
}

void Text::set_max_width(float max_width){
	_graphic_needs_update = true;
	_text_max_width = max_width;
}

float Text::get_max_width(){
	return _text_max_width;
}

void Text::set_wait_for_words(bool wait_for_words){
	_graphic_needs_update = true;
	_wait_for_words = wait_for_words;
	
}
bool Text::get_wait_for_words(){
	return _wait_for_words;
}

void Text::set_color(const vec4& color){
	_color = color;
}

vec4 Text::get_color(){
	return _color;
}

void Text::_update_graphic() {
	std::vector<float> verticies;
	std::vector<unsigned int> indicies;

	float pen_x = 0;
	float pen_y = 0;

	int char_count = 0;

	bool next_line_at_next_space = false;

	for (const uint8_t& character : _text) {

		if (_text_max_width) {
			if (pen_x > _text_max_width)
				next_line_at_next_space = true;
		}

		const glyph_info& character_info = _font->glyphs[character];

		float uv_width = character_info.x1 - character_info.x0;
		float uv_height = character_info.y1 - character_info.y0;

		float xpos = pen_x + character_info.x_off * _scale;
		float ypos = pen_y - (uv_height - character_info.y_off) * _scale;

		float w = uv_width * _scale;
		float h = uv_height * _scale;

		if (character == ' ') {
			if (next_line_at_next_space) {
				pen_x = 0;
				pen_y -= _font->glyphs['\n'].y1 * _scale;
				next_line_at_next_space = false;
				continue;
			}
		}

		if (character == '\n') {
			pen_x = 0;
			pen_y -= uv_height * _scale;
			next_line_at_next_space = false;
			continue;
		}

		float new_verticies[] = {
			xpos,		ypos,		0,	character_info.x0, 1 - character_info.y1,
			xpos + w,	ypos,		0,	character_info.x1, 1 - character_info.y1,
			xpos,		ypos + h,	0,	character_info.x0, 1 - character_info.y0,
			xpos + w,	ypos + h,	0,	character_info.x1, 1 - character_info.y0,
		};

		unsigned int new_indicies[] = {
			0 + char_count * 4, 1 + char_count * 4, 3 + char_count * 4,
			0 + char_count * 4, 3 + char_count * 4, 2 + char_count * 4,
		};

		for (float value : new_verticies)
			verticies.push_back(value);

		for (unsigned int value : new_indicies)
			indicies.push_back(value);

		pen_x += character_info.advance * _scale;
		char_count++;

	}

	ArrayBuffer_s arraybuffer;
	arraybuffer->load_buffer(verticies);
	arraybuffer->push_attribute(3);
	arraybuffer->push_attribute(2);

	IndexBuffer_s indexbuffer;
	indexbuffer->load_buffer(indicies, 3);

	Mesh_s text_mesh(arraybuffer, indexbuffer);
	graphic->load_model(text_mesh);
	//graphics_representation->set_uniform_all(default_program::basic_uniform_queue(scene, graphics_representation));
	//graphics_representation->set_uniform("texture_slot", 0);
	//graphics_representation->set_uniform("screen_resolution", (float*)&scene.camera.screen_width, (float*)&scene.camera.screen_height);
	
	_graphic_needs_update = false;
}

void Text::_initialize_default_renderer() {
	if (_default_renderer_initialized)
		return;
	_default_text_renderer = default_program::text_program_s();
	_default_renderer_initialized = true;
}

void Text::render(){
	if (!_custom_renderer_loaded && !_default_renderer_initialized)
		_initialize_default_renderer();

	if (_graphic_needs_update)
		_update_graphic();

	if (!_custom_renderer_loaded)
		graphic->load_program(_default_text_renderer);
	_font->_font_atlas.texture_slot = 0;
	_font->_font_atlas.bind();

	graphic->update_matrix();
	graphic->update_uniforms();
	graphic->draw(false);
}