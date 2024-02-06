#include "GL\glew.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Frame.h"
#include "Debuger.h"
#include <iostream>

#include "Gui.h"

bool is_glew_initialized = false;
bool is_glfw_initialized = false;
int fps_counter_batch = 180;
double frame_time_ms = 0;
double frame_rate_fps = 0;

// private
double old_time = glfwGetTime();
double old_time_accurate = 0;
int fps_counter_index = 0;
double seconds_total_batch = 0;

Frame::Frame(int width, int height, const std::string& name, int msaa, int swapinterval, bool depth_test, bool blend, bool face_culling, bool initialize_gui) :
	window_width(width), window_height(height), window_name(name), multisample(msaa), swapinterval(swapinterval), depth_test(depth_test), blend(blend), face_culling(face_culling), initialize_imgui(initialize_gui)
{
	multisample = msaa;

	window_width = width;
	window_height = height;

	if (!is_glfw_initialized) {
		glfwInit();
		is_glfw_initialized = true;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	}
	if(multisample)
		glfwWindowHint(GLFW_SAMPLES, multisample);

	window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr);
	glfwMakeContextCurrent(window);
	glfwSwapInterval(swapinterval);
	glfwGetWindowSize(window, &window_width, &window_height);
	glViewport(0, 0, window_width, window_height);
	
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height) {
		Frame& frame = *(Frame*)glfwGetWindowUserPointer(window);
		glViewport(0, 0, width, height);
		frame.window_width = width;
		frame.window_height = height;
	});

	if (!is_glew_initialized) {
		glewInit();
		is_glew_initialized = true;
	}

	if (face_culling)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);

	if (depth_test){
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
	}
	else
		glDisable(GL_DEPTH_TEST);

	if (blend) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	else
		glEnable(GL_BLEND);

	if (multisample)
		glEnable(GL_MULTISAMPLE);
	else
		glDisable(GL_MULTISAMPLE);

	if (initialize_imgui) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 330");
	}

	set_cursor_type(Arrow);
}

Frame::~Frame() {
	Gui2::gui_program = nullptr;
	Gui2::font = nullptr;

	glfwDestroyWindow(window);
	glfwTerminate();
	is_glew_initialized = false;
	is_glfw_initialized = false;
}

void Frame::clear_window(float red, float green, float blue, float alpha) {
	if (!is_glfw_initialized || !is_glew_initialized)
		return;

	
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	GLCall(glClearColor(red, green, blue, alpha));
	if (initialize_imgui) {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}
}

void Frame::display_performance(int batch_size) {
	if (batch_size != NULL && fps_counter_batch != batch_size)
		fps_counter_batch = batch_size;

	double now = glfwGetTime();
	double seconds_current = (now - old_time);
	if (fps_counter_index == fps_counter_batch) {
		frame_time_ms = seconds_total_batch / fps_counter_index * 1000;
		frame_rate_fps = 1 / ((seconds_total_batch / fps_counter_batch));
		std::string ms = std::to_string(frame_time_ms);
		std::string fps = std::to_string(frame_rate_fps);
		std::cout << "[Opengl Info] ms:" + ms.substr(0, ms.find(".") + 3) + " fps: " + fps.substr(0, fps.find(".") + 3) << std::endl;
		fps_counter_index = 0;
		seconds_total_batch = 0;
	}
	seconds_total_batch += seconds_current;
	fps_counter_index++;
	old_time = now;
}

	
double Frame::get_interval_ms() {
	if (old_time_accurate == 0)
		old_time_accurate = glfwGetTime();
	double now = glfwGetTime();
	double ms_current = (now - old_time_accurate) * 1000;
	old_time_accurate = now;
	return ms_current;
}

double Frame::get_time_sec() {
	return glfwGetTime();
}

bool Frame::is_running() {
	return !glfwWindowShouldClose(window);
}

bool first_handle = true;
double Frame::handle_window() {
	if (!first_handle)
		glfwSwapBuffers(window);
	glfwPollEvents();

	if (first_handle) {
		first_handle = false;
		get_interval_ms(); // reset it
	}

	Frame::CursorState new_cursor_state = Frame::Hover;
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		new_cursor_state = (Frame::CursorState)(new_cursor_state | Frame::LeftPressed);
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		new_cursor_state = (Frame::CursorState)(new_cursor_state | Frame::RightPressed);
	
	if (((_cursor_state & Frame::LeftPressed) != 0) && ((new_cursor_state & Frame::LeftPressed) == 0))
		new_cursor_state = (Frame::CursorState)(new_cursor_state | Frame::LeftReleased);
	if (((_cursor_state & Frame::RightPressed) != 0) && ((new_cursor_state & Frame::RightPressed) == 0))
		new_cursor_state = (Frame::CursorState)(new_cursor_state | Frame::RightReleased);

	_cursor_state = new_cursor_state;

	return get_interval_ms();
}

void Frame::set_viewport(int width, int height) {
	GLCall(glViewport(0, 0, width, height));
}

void Frame::set_viewport(glm::vec2 size) {
	set_viewport(size.x, size.y);
}

Vec2<int> Frame::get_cursor_position() {
	double mouse_x, mouse_y;
	glfwGetCursorPos(window, &mouse_x, &mouse_y);
	return Vec2<int>((int)mouse_x, (int)mouse_y);
}

void Frame::set_cursor_type(Frame::CursorType cursor_type) {
	if (_current_cursor_type == cursor_type)
		return;

	switch (cursor_type) {
		case Frame::Arrow:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
			break;
		case Frame::IBeam:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, glfwCreateStandardCursor(GLFW_IBEAM_CURSOR));
			break;
		case Frame::Crosshair:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, glfwCreateStandardCursor(GLFW_CROSSHAIR_CURSOR));
			break;
		case Frame::Hand:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, glfwCreateStandardCursor(GLFW_HAND_CURSOR));
			break;
		case Frame::HResize:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, glfwCreateStandardCursor(GLFW_HRESIZE_CURSOR));
			break;
		case Frame::VResize:
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			glfwSetCursor(window, glfwCreateStandardCursor(GLFW_VRESIZE_CURSOR));
			break;
		case Frame::Disabled:
			glfwSetCursor(window, glfwCreateStandardCursor(GLFW_ARROW_CURSOR));
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			break;
		}

	_current_cursor_type = cursor_type;
}

bool Frame::get_mouse_state(Frame::CursorState state) {
	return (Frame::CursorState)(_cursor_state & state) != 0;

}

bool Frame::get_key_press(Frame::Key key) {
	return glfwGetKey(window, (int)key);
}