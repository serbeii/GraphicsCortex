# include "Lights.h"
#include <string>
#include <iostream>


void Light::update_uniforms() {
	std::cout << "[WARNING] light.update_uniforms() executed" << std::endl;
}

void Light::update_uniform_queue() {
	_uniform_update_queue.update_uniforms();
}

AmbiantLight::AmbiantLight(const glm::vec3& color, Program_s renderer) :
	color(color)
{
	program = renderer.obj;
}

AmbiantLight::AmbiantLight(const glm::vec3& color) : 
	color(color) {}

void AmbiantLight::update_uniforms() {
	std::string uniform_name = shader_name + "[" + std::to_string(count) + "]";
	program->update_uniform(uniform_name + ".color", color.x, color.y, color.z);

	// temp
	count++;
	program->update_uniform(shader_name+"_count", count);
}

int AmbiantLight::count = 0;

DirectionalLight::DirectionalLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color, Program_s renderer) :
	position(position), direction(direction), color(color)
{
	program = renderer.obj;
	update_matricies();
}

DirectionalLight::DirectionalLight(const glm::vec3& position, const glm::vec3& direction, const glm::vec3& color) : 
	position(position), direction(direction), color(color)
{
	update_matricies();
}

void DirectionalLight::update_matricies() {
	light_view_matrix = glm::lookAt(position, position + direction, glm::vec3(0.0f, 1.0f, 0.0f));
	float scale = 300;
	light_projection_matrix = glm::ortho(-scale, scale, -scale, scale, -500.0f, 500.0f);
}

void DirectionalLight::update_uniforms() {
	std::string uniform_name = shader_name + "[" + std::to_string(count) + "]";
	program->update_uniform(uniform_name + ".direction", direction.x, direction.y, direction.z);
	program->update_uniform(uniform_name + ".color", color.x, color.y, color.z);

	// temp
	count++;
	program->update_uniform(shader_name + "_count", count);
}

int	DirectionalLight::count = 0;

PointLight::PointLight(const glm::vec3& position, const glm::vec3& color, float constant_term, float linear_term, float exponential_term, Program_s renderer) :
	position(position), color(color), constant_term(constant_term), linear_term(linear_term), exponential_term(exponential_term) {program = renderer.obj;}

void PointLight::update_uniforms() {
	std::string uniform_name = shader_name + "[" + std::to_string(count) + "]";
	program->update_uniform(uniform_name + ".position", position.x, position.y, position.z);
	program->update_uniform(uniform_name + ".color", color.x, color.y, color.z);
	program->update_uniform(uniform_name + ".constant_term", constant_term);
	program->update_uniform(uniform_name + ".linear_term", linear_term);
	program->update_uniform(uniform_name + ".exponential_term", exponential_term);

	// temp
	count++;
	program->update_uniform(shader_name + "_count", count);
}

int PointLight::count = 0;

SpotLight::SpotLight(const glm::vec3& position, const glm::vec3& direction,  const glm::vec3& color, float constant_term, float linear_term, float exponential_term, float angle, Program_s renderer) :
	position(position), direction(direction), color(color), constant_term(constant_term), linear_term(linear_term), exponential_term(exponential_term), cos_angle(glm::cos(glm::radians(angle))) {program = renderer.obj;}

void SpotLight::update_uniforms() {
	std::string uniform_name = shader_name + "[" + std::to_string(count) + "]";
	program->update_uniform(uniform_name + ".position", position.x, position.y, position.z);
	program->update_uniform(uniform_name + ".direction", direction.x, direction.y, direction.z);
	program->update_uniform(uniform_name + ".color", color.x, color.y, color.z);
	program->update_uniform(uniform_name + ".constant_term", constant_term);
	program->update_uniform(uniform_name + ".linear_term", linear_term);
	program->update_uniform(uniform_name + ".exponential_term", exponential_term);
	program->update_uniform(uniform_name + ".cos_angle", cos_angle);

	// temp
	count++;
	program->update_uniform(shader_name + "_count", count);
}

int SpotLight::count = 0;