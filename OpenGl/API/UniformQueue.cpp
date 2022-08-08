#include "UnifromQueue.h"

#include <iostream>


template <typename T>
void uniform_update<T>::update_uniform(Program& program) {
	if (data_amount == 1)
		program.update_uniform(uniform_name, data1);
	else if (data_amount == 2)
		program.update_uniform(uniform_name, data1, data2);
	else if (data_amount == 3)
		program.update_uniform(uniform_name, data1, data2, data3);
	else if (data_amount == 4)
		program.update_uniform(uniform_name, data1, data2, data3, data4);
	else {
		std::cout << "[Opengl Error] uniform_update.update_uniform(Program program) is called but uniform_update.data_amount is not between 1 and 4. \n";
		return;
	}
}

template<typename T>
void dynamic_uniform_update<T>::update_uniform(Program& program) {
	if (data_amount == 1)
		program.update_uniform(uniform_name, *data1);
	else if (data_amount == 2)
		program.update_uniform(uniform_name, *data1, *data2);
	else if (data_amount == 3)
		program.update_uniform(uniform_name, *data1, *data2, *data3);
	else if (data_amount == 4)
		program.update_uniform(uniform_name, *data1, *data2, *data3, *data4);
	else {
		std::cout << "[Opengl Error] dynamic_uniform_update.update_uniform(Program program) is called but dynamic_uniform_update.data_amount is not between 1 and 4. \n";
		return;
	}
}

void uniform_update_queue::add_uniform_update(uniform_update<int> uniform_update) {
	uniform_queue_int.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<char> uniform_update) {
	uniform_queue_char.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<bool> uniform_update) {
	uniform_queue_bool.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<glm::mat4> uniform_update) {
	uniform_queue_mat4.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<glm::mat3> uniform_update) {
	uniform_queue_mat3.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<glm::mat2> uniform_update) {
	uniform_queue_mat2.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<glm::vec4> uniform_update) {
	uniform_queue_vec4.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<glm::vec3> uniform_update) {
	uniform_queue_vec3.push_back(uniform_update);
}
void uniform_update_queue::add_uniform_update(uniform_update<glm::vec2> uniform_update) {
	uniform_queue_vec2.push_back(uniform_update);
}

// ------------------------------------------------------------------------------------------

void uniform_update_queue::add_uniform_update(dynamic_uniform_update<int> dynamic_uniform_update) {
	dynamic_uniform_queue_int.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<char> dynamic_uniform_update) {
	dynamic_uniform_queue_char.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<bool> dynamic_uniform_update) {
	dynamic_uniform_queue_bool.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<glm::mat4> dynamic_uniform_update) {
	dynamic_uniform_queue_mat4.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<glm::mat3> dynamic_uniform_update) {
	dynamic_uniform_queue_mat3.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<glm::mat2> dynamic_uniform_update) {
	dynamic_uniform_queue_mat2.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<glm::vec4> dynamic_uniform_update) {
	dynamic_uniform_queue_vec4.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<glm::vec3> dynamic_uniform_update) {
	dynamic_uniform_queue_vec3.push_back(dynamic_uniform_update);
}
void uniform_update_queue::add_uniform_update(dynamic_uniform_update<glm::vec2> dynamic_uniform_update) {
	dynamic_uniform_queue_vec2.push_back(dynamic_uniform_update);
}

void uniform_update_queue::update_uniforms(Program& program) {
	for (uniform_update update : uniform_queue_int)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_char)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_bool)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_mat4)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_mat3)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_mat2)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_vec4)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_vec3)
		update.update_uniform(program);
	for (uniform_update update : uniform_queue_vec2)
		update.update_uniform(program);

	for (dynamic_uniform_update update : dynamic_uniform_queue_int)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_char)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_bool)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_mat4)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_mat3)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_mat2)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_vec4)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_vec3)
		update.update_uniform(program);
	for (dynamic_uniform_update update : dynamic_uniform_queue_vec2)
		update.update_uniform(program);
}
