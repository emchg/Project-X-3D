#include "scene.hpp"

#include "cgp/opengl_include.hpp"
#include "third_party/glad/opengl33/glad.hpp"

#include <GLFW/glfw3.h>

using namespace cgp;

class Triangle {
private:
	cgp::vec3 v0{};
	float pad0{};
	cgp::vec3 v1{};
	float pad1{};
	cgp::vec3 v2{};
	float pad2{};

public:
	Triangle(vec3 v0, vec3 v1, vec3 v2) : v0{ v0 }, v1{ v1 }, v2{ v2 } {}
};

// Main initialization function called once at program startup
// Sets up the camera, 3D scene elements, and the image animation system
void scene_structure::initialize()
{
	
	std::cout << "Start function scene_structure::initialize()" << std::endl;

	// Set the behavior of the camera and its initial position
	// ********************************************** //
	camera_control.initialize(inputs, window); 
	camera_control.set_rotation_axis_z(); // camera rotates around z-axis
	//   look_at(camera_position, targeted_point, up_direction)
	camera_control.look_at(
		{ 5.0f, -4.0f, 3.5f } /* position of the camera in the 3D scene */,
		{0,0,0} /* targeted point in 3D scene */,
		{0,0,1} /* direction of the "up" vector */);

	camera_projection = camera_projection_perspective{
		50.0f * Pi/180, // Field of view
		1.0f,           // Aspect ratio
		0.01f,          // Depth min
		1000            // Depth max
	};


	// General information
	display_info();

	// Create 3D coordinate frame (x, y, z axes) for visual reference
	global_frame.initialize_data_on_gpu(mesh_primitive_frame());

	// Initialize the shapes of the scene
	// ***************************************** //

	gui.display_frame = true;

	mesh quadrangle_mesh{};
	float factor{ 1.f };
	quadrangle_mesh.position = { factor * vec3{ -1., -1., 0. }, factor * vec3{ 1., -1., 0.}, factor * vec3{ 1., 1., 0.}, factor * vec3{ -1., 1., 0. } };
	quadrangle_mesh.uv = { { 0., 0. }, { 1., 0. }, { 1., 1. }, { 0., 1. } };
	quadrangle_mesh.connectivity = { { 0, 1, 2 }, { 0, 2, 3 } };
	quadrangle_mesh.fill_empty_field();
	quad_drawable.initialize_data_on_gpu(quadrangle_mesh);
	opengl_shader_structure shader_raymarching{};
	shader_raymarching.load(
		project::path + "shaders/ray_tracer/ray_tracer.vert.glsl",
		project::path + "shaders/ray_tracer/ray_tracer.frag.glsl"
	);

	mesh tree_mesh{ mesh_load_file_obj(project::path + "assets/palm_tree/palm_tree.obj") };
	tree_mesh.translate(vec3{ 0.f, -0.f, -2.f });
	tree_mesh.fill_empty_field();
	tree_drawable.initialize_data_on_gpu(tree_mesh);

	std::vector<Triangle> triangles{ { { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f }, { 0.f, 0.f, 0.f } } };

	// std::vector<Triangle> triangles{};
	// for (const auto& t : tree_mesh.position) {
	// 	triangles.emplace_back(t.x, t.y, t.z);
	// }

	GLuint ssbo;
	glGenBuffers(1, &ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo);
	glBufferData(GL_SHADER_STORAGE_BUFFER, triangles.size() * sizeof(Triangle),
		triangles.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
	
	quad_drawable.shader = shader_raymarching;

	std::cout << "End function scene_structure::initialize()" << std::endl;
}




// This function is called permanently at every new frame
// Note that you should avoid having costly computation and large allocation defined there. This function is mostly used to call the draw() functions on pre-existing data.
void scene_structure::display_frame()
{
	// Set the light to the current position of the camera
    camera_projection.aspect_ratio = window.aspect_ratio();
	environment.camera_projection = camera_projection.matrix();
	environment.camera_view = camera_control.camera_model.matrix_view();
	environment.light = camera_control.camera_model.position();
	

	// Draw the 3D reference frame axes if enabled
	if (gui.display_frame)
		draw(global_frame, environment);

	draw(quad_drawable, environment);
	if (gui.display_wireframe)
		draw_wireframe(quad_drawable, environment, { 1.f, 0.f, 0.f });

	draw(tree_drawable, environment);
	if (gui.display_wireframe)
		draw_wireframe(tree_drawable, environment, { 1.f, 0.f, 0.f });

	// Update time
	timer.update();
}


void scene_structure::display_gui()
{
	ImGui::Checkbox("Frame", &gui.display_frame);
	ImGui::Checkbox("Wireframe", &gui.display_wireframe);
}




void scene_structure::mouse_move_event()
{
	if (!inputs.keyboard.shift)
		camera_control.action_mouse_move();
	
}
void scene_structure::mouse_click_event()
{
	camera_control.action_mouse_click();
}
void scene_structure::keyboard_event()
{
	camera_control.action_keyboard();
}
void scene_structure::idle_frame()
{
	camera_control.idle_frame();
	
}

void scene_structure::display_info() const
{
	std::cout << "\nCAMERA CONTROL:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << camera_control.doc_usage() << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;


	std::cout << "\nSCENE INFO:" << std::endl;
	std::cout << "-----------------------------------------------" << std::endl;
	std::cout << "Example of scene to start a project." << std::endl;
	std::cout << "-----------------------------------------------\n" << std::endl;
}
