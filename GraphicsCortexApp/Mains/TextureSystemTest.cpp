#include "GraphicsCortex.h"

int main(){
	Frame frame(800, 600, "GraphicsCortex", 0, 0, true, true, false, Frame::CallbackLevel::LOW, false);
	Scene scene(frame);
	scene.camera->max_distance = 1000;
	scene.camera->fov = 70;

	std::shared_ptr<UnorderedMaterial> material = std::make_shared<UnorderedMaterial>();
	std::shared_ptr<Program> program = default_program::flatcolor_program_s();
	std::shared_ptr<Graphic> graphic = std::make_shared<Graphic>(default_geometry::cube());

	std::shared_ptr<Program> custom_program = std::make_shared<Program>(Shader("../GraphicsCortex/Custom Shaders/Texture.vert", "../GraphicsCortex/Custom Shaders/Texture.frag"));

	graphic->load_program(custom_program);
	graphic->load_material(material);
	scene.add(graphic);

	Image image1("../GraphicsCortex/Images/orange.png", 4, true);
	Image image2("../GraphicsCortex/Images/cobble.png", 4, true);

	BindlessMaterial bindless_material(custom_program);
	std::shared_ptr<Texture2D> my_texture = std::make_shared<Texture2D>(image1.get_width(), image1.get_height(), TextureBase2::ColorTextureFormat::RGBA8, 1, 0);
	bindless_material.add_texture("color_texture", my_texture);

	my_texture->load_data_async("../GraphicsCortex/Images/orange.png", TextureBase2::ColorFormat::RGBA, TextureBase2::Type::UNSIGNED_BYTE, 0);
	my_texture->wait_async_load();

	while (frame.is_running()) {
		double deltatime = frame.handle_window();
		frame.clear_window();
		frame.display_performance(180);

		bindless_material.update_uniforms();
		scene.render();


		scene.camera->handle_movements(frame.window, deltatime);
	}

}