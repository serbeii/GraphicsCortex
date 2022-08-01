#include "ShaderCompiler.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <GL\glew.h>
#include "Debuger.h"

unsigned int current_binded_program = 0;

Shader::Shader() { ; }

Shader::Shader(const std::string& target_file)
{
	read_shader(target_file);
}
Shader::Shader(const std::string& vertex_target_file, const std::string& fragment_target_file) {
	read_shader(vertex_target_file);
	read_shader(fragment_target_file);
}
Shader::Shader(const std::string& vertex_target_file, const std::string& geometry_terget_file, const std::string& fragment_target_file) {
	read_shader(vertex_target_file);
	read_shader(geometry_terget_file);
	read_shader(fragment_target_file);
}

void Shader::read_shader(const std::string& target_file) {
	std::string type = "";
	std::ifstream file(target_file);
	std::string line;

	while (std::getline(file, line)) {
		if (line.find("#<vertex shader>") != std::string::npos) {
			type = "vertex";
			continue;
		}
		else if (line.find("#<fragment shader>") != std::string::npos) {
			type = "fragment";
			continue;
		}
		else if (line.find("#<geometry shader>") != std::string::npos) {
			type = "geometry";
			continue;
		}
		if (type == "vertex") {
			vertex_shader += line + '\n';
		}
		else if (type == "fragment") {
			fragment_shader += line + '\n';
		}
		else if (type == "geometry") {
			geometry_shader += line + '\n';
		}
	}
}

unsigned int Program::compile_shader(unsigned int type, const std::string& shader_source) {
	unsigned int shader_id = glCreateShader(type);
	const char* shader_code_pointer = &shader_source[0];
	GLCall(glShaderSource(shader_id, 1, &shader_code_pointer, nullptr));

	std::string type_string = std::to_string(type);
	if (type == GL_VERTEX_SHADER)
		type_string = "vertex shader";
	else if (type == GL_FRAGMENT_SHADER)
		type_string = "fragment shader";
	else if (type == GL_GEOMETRY_SHADER)
		type_string = "geometry shader";

	std::cout << "compiling the shader of type " << type_string << '\n';

	GLCall(glCompileShader(shader_id));

	int compile_status;
	GLCall(glGetShaderiv(shader_id, GL_COMPILE_STATUS, &compile_status));
	if (compile_status != GL_TRUE) {
		char* compile_error_msg = (char*)malloc(1024 * sizeof(char));
		GLCall(glGetShaderInfoLog(shader_id, 1024, nullptr, compile_error_msg));
		std::cout << "shader compile error with massage: " << compile_error_msg << '\n';
	}
	return shader_id;
}

Program::Program() {
	id = 0;
}
Program::Program(const std::string& vertex_shader_code, const std::string& fragment_shader_code) {
	compile(vertex_shader_code, fragment_shader_code);
}
Program::Program(const std::string& vertex_shader_code, const std::string& geometry_shader_code, const std::string& fragment_shader_code) {
	compile(vertex_shader_code, geometry_shader_code, fragment_shader_code);
}

void Program::_detect_and_define_all_uniforms(const std::string& shader_code) {
	// detect uniform definitions
	std::stringstream ss(shader_code);
	std::string line;
	while (std::getline(ss, line)) {
		int location = line.find("uniform");
		if (location != std::string::npos) {

			// find the name of the uniform
			// uniform type NAME;
			// uniform type NAME = 0;
			// _ws0   _ws1 _ws2 _ws3		ws: white space: ' ', '=', ';'

			// NAME = string (ws2 + 1 : ws3)

			int space_counter = 0;
			int begin, end;
			for (int iterate = 0; space_counter < 3; iterate++) {
				char current_character = line[location + iterate];
				if (current_character == ' ' || current_character == '=' || current_character == ';' || current_character == '\t') {
					space_counter++;
					if (space_counter == 2)
						begin = location + iterate;
					else if (space_counter == 3) {
						end = location + iterate;
					}
				}
			}
			std::string uniform_name = line.substr(begin + 1, end - begin - 1);
			// uniforms can also be arrays
			// uniform type name[number];
			int array_bracet_begin = uniform_name.find('[');
			int array_bracet_end = uniform_name.find(']');
			if(array_bracet_begin == std::string::npos)
				define_uniform(uniform_name);
			else {
				int array_size = std::stoi(uniform_name.substr(array_bracet_begin + 1, array_bracet_end - array_bracet_begin - 1));
				for (int i = 0; i < array_size; i++) {
					std::string new_uniform_name = uniform_name.substr(0, array_bracet_begin + 1) + std::to_string(i) + uniform_name.substr(array_bracet_end);
					define_uniform(new_uniform_name);
				}
			}
		}
	}
}

void Program::compile(const std::string& vertex_shader_code, const std::string& fragment_shader_code) {
	id = glCreateProgram();
	unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_code);
	unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

	GLCall(glAttachShader(id, vertex_shader));
	GLCall(glAttachShader(id, fragment_shader));
	GLCall(glLinkProgram(id));

	GLCall(glDeleteShader(vertex_shader));
	GLCall(glDeleteShader(fragment_shader));

	_detect_and_define_all_uniforms(vertex_shader_code);
	_detect_and_define_all_uniforms(fragment_shader_code);
}

void Program::compile(const std::string& vertex_shader_code, const std::string& geometry_shader_code, const std::string& fragment_shader_code){
	id = glCreateProgram();
	unsigned int vertex_shader = compile_shader(GL_VERTEX_SHADER, vertex_shader_code);
	unsigned int geometry_shader = compile_shader(GL_GEOMETRY_SHADER, geometry_shader_code);
	unsigned int fragment_shader = compile_shader(GL_FRAGMENT_SHADER, fragment_shader_code);

	GLCall(glAttachShader(id, vertex_shader));
	GLCall(glAttachShader(id, geometry_shader));
	GLCall(glAttachShader(id, fragment_shader));
	GLCall(glLinkProgram(id));

	GLCall(glDeleteShader(vertex_shader));
	GLCall(glDeleteShader(geometry_shader));
	GLCall(glDeleteShader(fragment_shader));

	_detect_and_define_all_uniforms(vertex_shader_code);
	_detect_and_define_all_uniforms(geometry_shader_code);
	_detect_and_define_all_uniforms(fragment_shader_code);
}

void Program::bind() {
	if ((int)id != (int)current_binded_program){
		GLCall(glUseProgram(id));
	}
	current_binded_program = id;
}
void Program::unbind() {
	if (0 != current_binded_program){
		GLCall(glUseProgram(0));
	}
	current_binded_program = 0;
}

// UNIFORMS
void Program::define_uniform(std::string name) {
	if (uniforms.find(name) == uniforms.end())
		GLCall(uniforms.insert(std::pair<std::string, unsigned int>(name, glGetUniformLocation(this->id, name.c_str()))));
}

void Program::update_uniform(std::string name, glm::mat4 a) {
	if (name.find('.') != std::string::npos)
		define_uniform(name);
	bind();
	GLCall(glUniformMatrix4fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(a)));
}
void Program::update_uniform(std::string name, glm::mat3 a) {
	if (name.find('.') != std::string::npos)
		define_uniform(name);
	bind();
	GLCall(glUniformMatrix3fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(a)));
}
void Program::update_uniform(std::string name, glm::mat2 a) {
	if (name.find('.') != std::string::npos)
		define_uniform(name);
	bind();
	GLCall(glUniformMatrix2fv(uniforms[name], 1, GL_FALSE, glm::value_ptr(a)));
}

void Program::update_uniform(std::string name, glm::vec4 a) {
	if (name.find('.') != std::string::npos)
		define_uniform(name);
	bind();
	GLCall(glUniform4fv(uniforms[name], 1, glm::value_ptr(a)));
}
void Program::update_uniform(std::string name, glm::vec3 a) {
	if (name.find('.') != std::string::npos)
		define_uniform(name);
	bind();
	GLCall(glUniform3fv(uniforms[name], 1, glm::value_ptr(a)));
}
void Program::update_uniform(std::string name, glm::vec2 a) {
	if (name.find('.') != std::string::npos)
		define_uniform(name);
	bind();
	GLCall(glUniform2fv(uniforms[name], 1, glm::value_ptr(a)));
}

// shortcuts
void Program::update_uniform(int shortcut_index, glm::mat4 a) {
	bind();
	GLCall(glUniformMatrix4fv(uniform_id_shortcuts[shortcut_index], 1, GL_FALSE, glm::value_ptr(a)));
}
void Program::update_uniform(int shortcut_index, glm::mat3 a) {
	bind();
	GLCall(glUniformMatrix3fv(uniform_id_shortcuts[shortcut_index], 1, GL_FALSE, glm::value_ptr(a)));
}
void Program::update_uniform(int shortcut_index, glm::mat2 a) {
	bind();
	GLCall(glUniformMatrix2fv(uniform_id_shortcuts[shortcut_index], 1, GL_FALSE, glm::value_ptr(a)));
}

void Program::update_uniform(int shortcut_index, glm::vec4 a) {
	bind();
	GLCall(glUniform4fv(uniform_id_shortcuts[shortcut_index], 1, glm::value_ptr(a)));
}
void Program::update_uniform(int shortcut_index, glm::vec3 a) {
	bind();
	GLCall(glUniform3fv(uniform_id_shortcuts[shortcut_index], 1, glm::value_ptr(a)));
}
void Program::update_uniform(int shortcut_index, glm::vec2 a) {
	bind();
	GLCall(glUniform2fv(uniform_id_shortcuts[shortcut_index], 1, glm::value_ptr(a)));
}
