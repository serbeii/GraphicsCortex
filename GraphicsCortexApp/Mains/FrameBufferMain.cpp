#include "GraphicsCortex.h"

int main() {
	Frame frame(1920, 1080, "FrameBuffer App", 4, 0, true, true, true, Frame::CallbackLevel::LOW, false);
	Scene scene(frame);
	scene.camera->fov = 110;
	scene.camera->max_distance = 1000;

	std::shared_ptr<Program> bindless_program = std::make_shared<Program>(Shader("../GraphicsCortex/Source/GLSL/TextureArray.vert", "../GraphicsCortex/Source/GLSL/TextureArray.frag"));
	std::shared_ptr<Mesh> dragon_mesh;
	std::shared_ptr<BindlessMaterial> dragon_material;
	{
		Model dragon_model("../GraphicsCortex/Models/dragon_new/dragon_new.fbx", 1, Model::ALL);
		dragon_mesh = std::make_shared<Mesh>(dragon_model);
		dragon_material = AssetImporter::generate_material("../GraphicsCortex/Models/dragon_new/dragon_new.fbx", bindless_program);
	}
	AssetImporter::clear_ram_all();
	std::shared_ptr<Graphic> dragon = std::make_shared<Graphic>(dragon_mesh, dragon_material);
	dragon->rotation = glm::vec3(-glm::pi<float>() / 2, 0, 0);

	scene.add(dragon);

	std::shared_ptr<TextureCubeMap> cubemap = std::make_shared<TextureCubeMap>(2048, 2048, TextureCubeMap::ColorTextureFormat::RGBA8, 1, 0);
	cubemap->load_data_async(TextureCubeMap::Face::RIGHT,   "../GraphicsCortex/Images/CubeMap/Sky/px.jpg", TextureCubeMap::ColorFormat::RGBA, TextureCubeMap::Type::UNSIGNED_BYTE, 0);
	cubemap->load_data_async(TextureCubeMap::Face::LEFT,    "../GraphicsCortex/Images/CubeMap/Sky/nx.jpg", TextureCubeMap::ColorFormat::RGBA, TextureCubeMap::Type::UNSIGNED_BYTE, 0);
	cubemap->load_data_async(TextureCubeMap::Face::UP,      "../GraphicsCortex/Images/CubeMap/Sky/py.jpg", TextureCubeMap::ColorFormat::RGBA, TextureCubeMap::Type::UNSIGNED_BYTE, 0);
	cubemap->load_data_async(TextureCubeMap::Face::DOWN,    "../GraphicsCortex/Images/CubeMap/Sky/ny.jpg", TextureCubeMap::ColorFormat::RGBA, TextureCubeMap::Type::UNSIGNED_BYTE, 0);
	cubemap->load_data_async(TextureCubeMap::Face::FRONT,   "../GraphicsCortex/Images/CubeMap/Sky/pz.jpg", TextureCubeMap::ColorFormat::RGBA, TextureCubeMap::Type::UNSIGNED_BYTE, 0);
	cubemap->load_data_async(TextureCubeMap::Face::BACK,    "../GraphicsCortex/Images/CubeMap/Sky/nz.jpg", TextureCubeMap::ColorFormat::RGBA, TextureCubeMap::Type::UNSIGNED_BYTE, 0);
	cubemap->wait_async_load();

	//glProgramUniformHandleui64ARB(bindless_program->id, glGetUniformLocation(bindless_program->id, "cubemap"), cubemap->texture_handle);

	//std::shared_ptr<Texture2D> color_texture = std::make_shared<Texture2D>(1920, 1080, Texture2D::ColorTextureFormat::RGBA8, 1, 0, 4);
	//std::shared_ptr<Texture2D> depth_stencil_texture = std::make_shared<Texture2D>(1920, 1080, Texture2D::DepthStencilTextureFormat::DEPTH24_STENCIL8, 1, 0, 4);
	std::shared_ptr<Renderbuffer2> color_texture = std::make_shared<Renderbuffer2>(1920, 1080, Renderbuffer2::ColorTextureFormat::RGBA8, 0);
	std::shared_ptr<Renderbuffer2> depth_stencil_texture = std::make_shared<Renderbuffer2>(1920, 1080, Renderbuffer2::DepthStencilTextureFormat::DEPTH24_STENCIL8, 0);
	Framebuffer2 framebuffer;
	framebuffer.attach_color(0, color_texture);
	framebuffer.attach_depth_stencil(depth_stencil_texture);
	framebuffer.activate_draw_buffer(0);

	while (frame.is_running()) {
		framebuffer.bind_draw();
		double deltatime = frame.handle_window();
		frame.clear_window();
		frame.display_performance(180);

		scene.camera->handle_movements(frame.window, deltatime);
		scene.render();

		framebuffer.blit_to_screen(0, 0, 1920, 1080, 0, 0, 1920, 1080, Framebuffer2::Channel::COLOR, Framebuffer2::Filter::NEAREST);
	}
}