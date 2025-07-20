#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "opengl.h"
#include "texture.h"
#include "material.h"
#include "model.h"
#include "map.h"
#include "math.h"
#include "renderer.h"

//
// --- Global variables ---
//
Camera camera;
Mouse mouse;
Frame_Time frame_time;
Screen screen;
GPU_Info gpu_info;

bool imgui_draw_edit_window = true;
bool imgui_draw_demo_window = false;
bool freeze_light_change = false;
bool freeze_camera = false;

//
// --- Callbacks ---
//
void framebuffer_size_callback(GLFWwindow* window, int new_screen_width, int new_screen_height) {
	screen.width = new_screen_width;
	screen.height = new_screen_height;
	screen.aspect_ratio = (float)new_screen_width / (float)new_screen_height;
	glViewport(0, 0, new_screen_width, new_screen_height);
	printf("[%f] - New window size: %dx%d\n", frame_time.current, screen.width, screen.height);
}

void mouse_callback(GLFWwindow* window, double new_mouse_x, double new_mouse_y) {
	float offset_x = new_mouse_x - mouse.x;
	float offset_y = mouse.y - new_mouse_y; // Reversed, since  y-coordinates go from bottom to top.
	mouse.x = new_mouse_x;
	mouse.y = new_mouse_y;

	offset_x *= mouse.sensitivity;
	offset_y *= mouse.sensitivity;

	// In 'Cursor mode' we don't move camera.
	if (!mouse.cursor_mode) {
		camera.yaw += offset_x;
		camera.pitch += offset_y;

		if (camera.pitch > PITCH_MAX_ANGLE)
			camera.pitch = PITCH_MAX_ANGLE;
		if (camera.pitch < -PITCH_MAX_ANGLE)
			camera.pitch = -PITCH_MAX_ANGLE;

		glm::vec3 direction;
		direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
		direction.y = sin(glm::radians(camera.pitch));
		direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));

		camera.front = glm::normalize(direction);
	}
}

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y) {
	if (!freeze_camera) {
		camera.fov -= (float) offset_y;
		printf("[%f] - Camera FOV: %f\n", frame_time.current, camera.fov);
	}
}

void process_input(GLFWwindow* window) {
	if (freeze_camera) return;
	// Temp.
	float camera_speed = 3.0f * frame_time.delta;
	glm::vec3 cross_front_up = glm::cross(camera.front, camera.up);
	glm::vec3 camera_right = glm::normalize(cross_front_up);

	//      if(glfwGetKey(window, GLFW_KEY)) == GLFW_PRESS)
	//  or: if(glfwGetKey(window, GLFW_KEY))
	//
	// NOTE: It doesn't work like GLFW key press callback function
	// where there is additional GLFW_REPEAT and GLFW_PRESS is treated
	// as a single press. Here, GLFW_PRESS updates every frame.
	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera.position += camera.front * camera_speed;
	}

	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera.position -= camera.front * camera_speed;
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera.position -= camera_right * camera_speed;
	}

	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera.position += camera_right * camera_speed;
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		// Move camera up relative to the world, not the camera view.
		// camera.position -= camera.up * camera_speed;
		camera.position -= glm::vec3(0.0f, 1.0f, 0.0f) * camera_speed;
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		// Move camera up relative to the world, not the camera view.
		// camera.position += camera.up * camera_speed;
		camera.position += glm::vec3(0.0f, 1.0f, 0.0f) * camera_speed;
	}
}

void single_press_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
		imgui_draw_edit_window = !imgui_draw_edit_window;
		printf("[%f] - 'Edit' window mode: %s\n", frame_time.current, (imgui_draw_edit_window) ? "SHOW" : "HIDE");
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		mouse.cursor_mode = !mouse.cursor_mode;

		if (mouse.cursor_mode) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		printf("[%f] - Cursor mode: %s\n", frame_time.current, (mouse.cursor_mode) ? "ON" : "OFF");
	}
}

void glfw_error_callback(int error_code, const char* description) {
	printf("GLFW ERROR: %s (Error code: %d)\n", description, error_code);
}

static void
load_phong_lighting_shader() {
	Array< Renderer_Shader_Stage * > stages  = array_new< Renderer_Shader_Stage * >( sys_allocator, RendererShaderKind_COUNT );

	// Vertex
	array_add( &stages, renderer_load_shader_stage(
		"phong_vertex",
		RendererShaderKind_Vertex,
		"resources/shaders/phong_vertex.glsl"
	) );

	// Fragment
	array_add( &stages, renderer_load_shader_stage(
		"phong_fragment",
		RendererShaderKind_Fragment,
		"resources/shaders/phong_fragment.glsl"
	) );

	Renderer_Shader_Program *shader = renderer_create_and_compile_shader_program(
		"phong_program",
		array_view( &stages )
	);

	/* Attributes */

	Array< Renderer_Vertex_Attribute > *attributes = &shader->vertex_attributes;

	array_add( attributes, Renderer_Vertex_Attribute {
		.name = "texture_uv",
		.index = 0,
		.elements = 2,
		.data_type = RendererDataType_f32,
		.normalize = false,
		.active = true
	} );

	/* Uniforms */

	Array< Renderer_Uniform > *phong_uniforms = &shader->uniforms;

	/* Vertex stage uniforms */

	/* Fragment stage uniforms */

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "gbuffer_position",
		.data_type = RendererDataType_Matrix4x4_f32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "gbuffer_normal",
		.data_type = RendererDataType_Matrix4x4_f32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "gbuffer_diffuse_specular",
		.data_type = RendererDataType_Matrix4x4_f32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "view_position",
		.data_type = RendererDataType_Vector3_f32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "ambient",
		.data_type = RendererDataType_Vector3_f32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "lights",
		.data_type = RendererDataType_Vector3_f32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "shininess_exponent",
		.data_type = RendererDataType_f32,
	} );

	renderer_shader_program_update_uniform_locations( shader );
}

void load_shaders() {
	load_phong_lighting_shader();
}

void load_texture( StringView_ASCII name, StringView_ASCII file_path, bool upload_to_renderer ) {
	Texture_ID texture_id = texture_load_from_file( name, file_path );
	if ( upload_to_renderer )
		renderer_texture_upload( texture_id );
}

void load_textures() {
	load_texture( "bark_diffuse", "resources/textures/bark_diffuse_x3072_expt1-255.png", true );
	load_texture( "bark_normal", "resources/textures/bark_normal_x3072_expt1-255_gauss-bilat.png", true );

	load_texture( "dried-soil_diffuse", "resources/textures/dried_soil_diffuse_x3072_expt1-190.png", true );
	load_texture( "dried-soil_normal", "resources/textures/dried_soil_normal_x3072_expt1-190.png", true );

	load_texture( "rocks-medium_diffuse", "resources/textures/rocks-medium_diffuse_x3072_expt1-394_flat.png", true );
	load_texture( "rocks-medium_normal", "resources/textures/rocks-medium_normal_x3072_expt1-394_flat.png", true );
}

void create_materials() {
	Renderer_Shader_Program *shader = renderer_find_shader_program( "phong_program" );
	Assert( shader );
	Texture_ID diffuse;
	Texture_ID normal;
	Texture_ID specular = INVALID_TEXTURE_ID;

	diffuse = texture_find( "bark_diffuse" );
	normal = texture_find( "bark_normal" );
	Assert( diffuse != INVALID_TEXTURE_ID );
	Assert( normal != INVALID_TEXTURE_ID );
	material_create( "bark", shader, diffuse, normal, specular );

	diffuse = texture_find( "dried-soil_diffuse" );
	normal = texture_find( "dried-soil_normal" );
	specular = INVALID_TEXTURE_ID;
	Assert( diffuse != INVALID_TEXTURE_ID );
	Assert( normal != INVALID_TEXTURE_ID );
	material_create( "dried-soil", shader, diffuse, normal, specular );

	diffuse = texture_find( "rocks-medium_diffuse" );
	normal = texture_find( "rocks-medium_normal" );
	Assert( diffuse != INVALID_TEXTURE_ID );
	Assert( normal != INVALID_TEXTURE_ID );
	material_create( "rocks-medium", shader, diffuse, normal, specular );
}

int main()
{
	GLFWwindow* window;

	glfwSetErrorCallback(glfw_error_callback);

	if (glfwInit()) {
		printf("GLFW initialized.\n");
	} else {
		printf("ERROR: Failed to initialize GLFW!\n");
		ASSERT(false);
		exit(EXIT_FAILURE);
	}

	// OpenGL 4.6, Core profile.
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#if QLIGHT_DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	// Open a window and create its OpenGL context.
	// Put  glfwGetPrimaryMonitor()  after the window's name to make it fullscreen.
	// ... "name", glfwGetPrimaryMonitor(), NULL);
	GLFWmonitor* monitor = NULL;
	window = glfwCreateWindow(screen.width, screen.height, "OpenGL", monitor, NULL);
	if (window) {
		printf("GLFW Window created. (%dx%d, %s)\n", screen.width, screen.height, (monitor) ? "Fullscreen" : "Windowed");
	} else {
		printf("ERROR: Failed to open GLFW window!\n");
		ASSERT(false);
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(window);

	// In the 2nd argument we are telling GLFW
	// the function, that will execute whenever
	// frame buffer changes (when window changes its size)
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	// Callback function for processing single press.
	// Keys which are being listened continuously are
	// in process_input().
	glfwSetKeyCallback(window, single_press_key_callback);

	// Tell GLFW to capture mouse movements and don't draw the cursor.
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(window, mouse.x, mouse.y);

	// Vertical sync (VSync).
	// 1 - sync to a monitor refresh rate (60Hz = 60fps)
	// 2 - 1/2 of a monitor refresh rate (60Hz = 30fps)
	// 3 - 1/3...
	glfwSwapInterval(1);

	if (glewInit() == GLEW_OK) {
		printf("GLEW initialized.\n");
	} else {
		printf("ERROR: Failed to initialize GLEW!\n");
		ASSERT(false);
		exit(EXIT_FAILURE);
	}

	textures_init();
	materials_init();
	models_init();
	renderer_init();
	maps_init();

	load_textures();
	load_shaders();
	create_materials();

	map_change( "empty" );

	Model_ID model_cube_id = model_load_from_file( "cube", "resources/models/cube.obj" );
	renderer_model_meshes_upload( model_cube_id );
	Model *model_cube = model_instance( model_cube_id );
	Mesh_ID cube_mesh_id = model_cube->meshes.data[ 0 ];
	Mesh *cube_mesh = mesh_instance( cube_mesh_id );
	cube_mesh->material_id = material_find( "rocks-medium" );

	Map *map = map_current();

	Renderer_Camera fpv_camera = {
		.position = Vector3_f32 { 0.0f, 0.0f, 3.0f },
		.rotation = Quaternion { 0.0f, 0.0f, 0.0f, 1.0f },
		.direction_front = WORLD_DIRECTION_FRONT,
		.direction_up = WORLD_DIRECTION_UP,
		.view_matrix = Matrix4x4_f32(),
		.projection_matrix = Matrix4x4_f32(),
		.fov = 90.0f,
		.near_clip_plane_distance = 0.01f,
		.far_clip_plane_distance = 200.0f,
		.aspect_ratio = 1920.0f / 1080.0f
	};

	renderer_set_view_matrix_pointer( &fpv_camera.view_matrix );
	renderer_set_projection_matrix_pointer( &fpv_camera.projection_matrix );

	printf(
		"GPU Vendor: " StringViewFormat ", GPU Renderer: " StringViewFormat "\n",
		StringViewArgument( renderer_device_vendor() ),
		StringViewArgument( renderer_device_name() )
	);

	/* DEMO SCENE START */
	// const int vertices_amount = 4 * (3 + 3);
	// const int indices_amount = 2 * 3;

//	/*
	float vertices[] = {
		// Position           Normal
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
		-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};

	Shader_Source lighting_shader_source = parse_shader("resources/shaders/lighting.glsl");
	unsigned int lighting_shader = create_shader(lighting_shader_source.vertex_shader, lighting_shader_source.fragment_shader);
	if (lighting_shader) {
		printf("Shader 'lighting' loaded.\n");
	} else {
		printf("Failed to load 'lighting' shader!\n");
	}

	Shader_Source light_object_shader_source = parse_shader("resources/shaders/light_object.glsl");
	unsigned int light_object_shader = create_shader(light_object_shader_source.vertex_shader, light_object_shader_source.fragment_shader);
	if (light_object_shader) {
		printf("Shader 'light_object' loaded.\n");
	} else {
		printf("Failed to load 'light_object' shader!\n");
	}

	unsigned int vertex_buffer_object;
	glGenBuffers(1, &vertex_buffer_object);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_object);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	//
	// --- Cube ---
	//
	unsigned int object_vertex_array_object;
	glGenVertexArrays(1, &object_vertex_array_object);
	glBindVertexArray(object_vertex_array_object);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	//
	// --- Light cube ---
	//
	unsigned light_object_vertex_array_object;
	glGenVertexArrays(1, &light_object_vertex_array_object);
	glBindVertexArray(light_object_vertex_array_object);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
	glEnableVertexAttribArray(0);

	//
	// --- Coordinate Systems (3D Rendering) ---
	//
	glm::mat4 emissive_object_model = glm::mat4(1.0f);
	glm::vec3 emissive_object_position = glm::vec3(2.0f, 1.5f, -2.0f);
	float emissive_object_scale = 0.2f;

	emissive_object_model = glm::translate(emissive_object_model, emissive_object_position);
	emissive_object_model = glm::scale(emissive_object_model, glm::vec3(emissive_object_scale));

	glm::mat4 reflective_object_model = glm::mat4(1.0f);
	glm::vec3 reflective_object_position = glm::vec3(0.0f, 0.0f, 0.0f);
	float reflective_object_scale = 1.0f;

	reflective_object_model = glm::translate(reflective_object_model, reflective_object_position);
	reflective_object_model = glm::scale(reflective_object_model, glm::vec3(reflective_object_scale));

	glm::mat4 view = glm::mat4(1.0f);
	glm::mat4 projection;

	unsigned int lighting_model_location = glGetUniformLocation(lighting_shader, "model");
	unsigned int lighting_view_location = glGetUniformLocation(lighting_shader, "view");
	unsigned int lighting_projection_location = glGetUniformLocation(lighting_shader, "projection");

	unsigned int light_object_model_location = glGetUniformLocation(light_object_shader, "model");
	unsigned int light_object_view_location = glGetUniformLocation(light_object_shader, "view");
	unsigned int light_object_projection_location = glGetUniformLocation(light_object_shader, "projection");

	// Here we enable 'Z-buffer' to allow us render
	// 3D objects without overwriting already rendered pixels.
//	*/
	glEnable(GL_DEPTH_TEST);

	//
	// --- Colors ---
	//
//	/*
	glm::vec3 light_color(1.0f, 1.0f, 1.0f);

	glm::vec3 cube_ambient = glm::vec3(0.3f);
	glm::vec3 cube_diffuse; // Gets updated every frame.
	glm::vec3 cube_specular; // Gets updated every frame.
	float cube_shininess = 32.0f;

	unsigned int lighting_material_ambient_location = glGetUniformLocation(lighting_shader, "material.ambient");
	unsigned int lighting_material_diffuse_location = glGetUniformLocation(lighting_shader, "material.diffuse");
	unsigned int lighting_material_specular_location = glGetUniformLocation(lighting_shader, "material.specular");
	unsigned int lighting_material_shininess_location = glGetUniformLocation(lighting_shader, "material.shininess");

	glm::vec3 cube_ambient_strength = glm::vec3(0.1f);
	glm::vec3 cube_diffuse_strength = glm::vec3(0.5f);
	glm::vec3 cube_specular_strength = glm::vec3(0.3f);

	unsigned int lighting_light_ambient_location = glGetUniformLocation(lighting_shader, "light.ambient");
	unsigned int lighting_light_diffuse_location = glGetUniformLocation(lighting_shader, "light.diffuse");
	unsigned int lighting_light_specular_location = glGetUniformLocation(lighting_shader, "light.specular");

	unsigned int lighting_view_position_location = glGetUniformLocation(lighting_shader, "view_position");
	unsigned int lighting_light_position_location = glGetUniformLocation(lighting_shader, "light.position");

	unsigned int light_object_light_color_location = glGetUniformLocation(light_object_shader, "light_color");
//	*/

	/* DEMO SCENE END */

	//
	// --- ImGui ---
	//
	IMGUI_CHECKVERSION();
	auto imgui_context = ImGui::CreateContext();
	if (imgui_context) {
		printf("ImGui context created.\n");
	} else {
		printf("Failed to create ImGui context!\n");
	}
	ImGuiIO &imgui_io = ImGui::GetIO();

	// Disable windows state saving for now.
	imgui_io.IniFilename = NULL;
	imgui_io.WantSaveIniSettings = false;

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	// Temp.
	const char* glsl_version = "#version 330";
	ImGui_ImplOpenGL3_Init(glsl_version);
	ImGui::SetCurrentContext(imgui_context);

//	/*
	// glm::vec4 clear_color = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
	glm::vec4 clear_color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

	// Setup scene.
	// camera.position = glm::vec3(1.2f, -0.7f, 1.0f);
	camera.position = glm::vec3(0.0f, 0.0f, 3.0f);
	// camera.pitch = 32.0f;
	camera.pitch = 0.0f;
	// camera.yaw = -115.0f;
	camera.yaw = 0.0f;

	// @CopyPaste from `mouse_callback()`.
	// Update camera front vector.
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
		direction.y = sin(glm::radians(camera.pitch));
		direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));

		camera.front = glm::normalize(direction);
		// @TODO: Up vector should be updated too!!!
		// It seems "right" for now because in case of going up
		// we want to go up in world up vector, no the camera one.
		// The camera up vector always stays the same as world's,
		// resulting in "right" solution (which is totally wrong!).
		// camera.up =
	}
//	*/

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		projection = glm::perspective(glm::radians(camera.fov), screen.aspect_ratio, NEAR_CLIP_PLANE_DISTANCE, FAR_CLIP_PLANE_DISTANCE);
		view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

		projection = glm::perspective(radians(fpv_camera.fov), fpv_camera.aspect_ratio, fpv_camera.near_clip_plane_distance, fpv_camera.far_clip_plane_distance);
		float glm_proj[4][4] = {
			{ projection[0][0], projection[1][0], projection[2][0], projection[3][0] },
			{ projection[0][1], projection[1][1], projection[2][1], projection[3][1] },
			{ projection[0][2], projection[1][2], projection[2][2], projection[3][2] },
			{ projection[0][3], projection[1][3], projection[2][3], projection[3][3] },
		};
		fpv_camera.projection_matrix = projection_perspective( radians( fpv_camera.fov ), fpv_camera.aspect_ratio, fpv_camera.near_clip_plane_distance, fpv_camera.far_clip_plane_distance );
		auto my_projection = fpv_camera.projection_matrix;
		float my_proj[4][4] = {
			{ my_projection[0][0], my_projection[1][0], my_projection[2][0], my_projection[3][0] },
			{ my_projection[0][1], my_projection[1][1], my_projection[2][1], my_projection[3][1] },
			{ my_projection[0][2], my_projection[1][2], my_projection[2][2], my_projection[3][2] },
			{ my_projection[0][3], my_projection[1][3], my_projection[2][3], my_projection[3][3] },
		};
		fpv_camera.view_matrix = projection_view( fpv_camera.position, fpv_camera.position + fpv_camera.direction_front, fpv_camera.direction_up );

		/* DEMO SCENE RENDER */
		// --- Lighting ---
		// Cube
//		/*
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		glUseProgram(lighting_shader);

		if (!freeze_light_change) {
			light_color.r = abs(sin(frame_time.current * 2.0f));
			light_color.g = abs(sin(frame_time.current * 0.7f));
			light_color.b = abs(sin(frame_time.current * 1.3f));
		}

		// cube_ambient = light_color * cube_ambient_strength;
		cube_diffuse = light_color * cube_diffuse_strength;
		cube_specular = light_color * cube_specular_strength;

		glUniform3fv(lighting_material_ambient_location, 1, &cube_ambient[0]);
		glUniform3fv(lighting_material_diffuse_location, 1, &cube_diffuse[0]);
		glUniform3fv(lighting_material_specular_location, 1, &cube_specular[0]);
		glUniform1f(lighting_material_shininess_location, cube_shininess);

		glUniform3fv(lighting_light_ambient_location, 1, &cube_ambient_strength[0]);
		glUniform3fv(lighting_light_diffuse_location, 1, &cube_diffuse_strength[0]);
		glUniform3fv(lighting_light_specular_location, 1, &cube_specular_strength[0]);

		glUniform3fv(lighting_view_position_location, 1, &camera.position[0]);
		glUniform3fv(lighting_light_position_location, 1, &emissive_object_position[0]);

		glUniformMatrix4fv(lighting_projection_location, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(lighting_view_location, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(lighting_model_location, 1, GL_FALSE, &reflective_object_model[0][0]);

		glBindVertexArray(object_vertex_array_object);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glUseProgram(light_object_shader);
		glUniform3fv(light_object_light_color_location, 1, &light_color[0]);
		glUniformMatrix4fv(light_object_projection_location, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(light_object_view_location, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(light_object_model_location, 1, GL_FALSE, &emissive_object_model[0][0]);

		glBindVertexArray(light_object_vertex_array_object);
		glDrawArrays(GL_TRIANGLES, 0, 36);
//		*/
		/* DEMO SCENE RENDER */

		renderer_queue_draw_command(
			/*     mesh_id */ cube_mesh_id,
			/* material_id */ cube_mesh->material_id,
			/*   transform */ &model_cube->transform
		);

		renderer_draw_frame();

		//
		// --- ImGui Render ---
		//
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (imgui_draw_demo_window) {
			ImGui::ShowDemoWindow(&imgui_draw_demo_window);
		}

		if (imgui_draw_edit_window) {

			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Scene")) {
				ImGui::Checkbox("Show demo window", &imgui_draw_demo_window);

//				/*
				ImGui::TextUnformatted("CAMERA:");

				ImGui::InputFloat3("Position", &camera.position[0]);
				ImGui::DragFloat("Field of view", &camera.fov, 1.0f, 1.0f, 179.0f);
				ImGui::DragFloat("Pitch", &camera.pitch);
				ImGui::DragFloat("Yaw", &camera.yaw);
				ImGui::ColorEdit3("Clear color", &clear_color[0]);
				ImGui::Checkbox("Freeze", &freeze_camera);

				ImGui::Separator();
				ImGui::TextUnformatted("POINT LIGHT (LIGHT CUBE):");

				ImGui::ColorEdit3("Color", &light_color[0]);
				ImGui::Checkbox("Freeze light change", &freeze_light_change);

				ImGui::Separator();
				ImGui::TextUnformatted("REFLECTIVE OBJECT (OTHER CUBE):");

				ImGui::ColorEdit3("Ambient", &cube_ambient[0]);
				ImGui::ColorEdit3("Diffuse", &cube_diffuse[0]);
				ImGui::ColorEdit3("Specular", &cube_specular[0]);
				ImGui::ColorEdit3("Ambient strength", &cube_ambient_strength[0]);
				ImGui::ColorEdit3("Diffuse strength", &cube_diffuse_strength[0]);
				ImGui::ColorEdit3("Specular strength", &cube_specular_strength[0]);
				ImGui::InputFloat("Shininess", &cube_shininess);

//				*/
				ImGui::Separator();
				ImGui::TextUnformatted("RENDERER:");

				ImGui::Text("GPU Vendor: " StringViewFormat, StringViewArgument( renderer_device_vendor() ));
				ImGui::Text("GPU Name: " StringViewFormat, StringViewArgument( renderer_device_name() ));
				ImGui::Text("Frametime: %.3f ms/frame (%.1f FPS)", 1000.0f / imgui_io.Framerate, imgui_io.Framerate);

			}

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
		glFlush();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	exit(EXIT_SUCCESS);
}