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
#include "console.h"
#include "transform.h"
#include "camera.h"

#define QL_LOG_CHANNEL "App"
#include "log.h"

//
// --- Global variables ---
//
Camera *g_camera;
Mouse mouse;
Screen screen;

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
	// glViewport(0, 0, new_screen_width, new_screen_height);
	log_info( "New window size: %dx%d.",
		screen.width,
		screen.height
	);
}

static int g_event_index = 0;
void mouse_callback(GLFWwindow* window, double new_mouse_x, double new_mouse_y) {
	f32 offset_x = ( f32 )new_mouse_x - mouse.x;
	f32 offset_y = mouse.y - ( f32 )new_mouse_y; // Reversed, since  y-coordinates go from bottom to top.
	mouse.x = ( f32 )new_mouse_x;
	mouse.y = ( f32 )new_mouse_y;

	offset_x *= mouse.sensitivity;
	offset_y *= mouse.sensitivity;

	// In 'Cursor mode' we don't move camera.
	if (!mouse.cursor_mode) {
		Quaternion delta_pitch = quaternion_from_axis_angle( WORLD_DIRECTION_RIGHT, radians( -offset_y ) );
		Quaternion delta_yaw = quaternion_from_axis_angle( WORLD_DIRECTION_UP, radians( offset_x ) );
		Quaternion rotation_delta = quaternion_multiply( delta_pitch, delta_yaw );
		Quaternion *q;
		int i = g_event_index;
		q = &delta_pitch;
		log_info( "[%d] delta_pitch: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		q = &delta_yaw;
		log_info( "[%d] delta_yaw: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		q = &rotation_delta;
		log_info( "[%d] rotation_delta: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		q = &g_camera->rotation;
		log_info( "[%d] (before) camera->rotation: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		Vector4_f32 *v;
		v = &g_camera->view_matrix[ 0 ];
		log_info( "[%d] (before) camera->view[0]: [%.3f, %.3f, %.3f, %.3f]", i, v->x, v->y, v->z, v->w );
		v = &g_camera->view_matrix[ 1 ];
		log_info( "[%d] (before) camera->view[1]: [%.3f, %.3f, %.3f, %.3f]", i, v->x, v->y, v->z, v->w );
		v = &g_camera->view_matrix[ 2 ];
		log_info( "[%d] (before) camera->view[2]: [%.3f, %.3f, %.3f, %.3f]", i, v->x, v->y, v->z, v->w );
		v = &g_camera->view_matrix[ 3 ];
		log_info( "[%d] (before) camera->view[3]: [%.3f, %.3f, %.3f, %.3f]", i, v->x, v->y, v->z, v->w );
		camera_rotate_by_quaternion( g_camera, rotation_delta );
		log_info( "[%d] (after) camera->rotation: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		g_event_index++;
	}
}

void scroll_callback(GLFWwindow* window, double offset_x, double offset_y) {
	if (!freeze_camera) {
		camera_zoom( g_camera, ( f32 ) -offset_y );
		log_info( "Camera FOV: %.f.", g_camera->fov );
	}
}

void process_input(GLFWwindow* window) {
	if (freeze_camera) return;

	f32 dt = renderer_frame_time_delta();
	f32 speed = 3.0f * ( dt / 1000.0f ); // ms -> sec

	Vector3_f32 camera_right = camera_direction_right( g_camera );
	Vector3_f32 camera_up = camera_direction_up( g_camera );
	Vector3_f32 camera_forward = camera_direction_forward( g_camera );

	//      if(glfwGetKey(window, GLFW_KEY)) == GLFW_PRESS)
	//  or: if(glfwGetKey(window, GLFW_KEY))
	//
	// NOTE: It doesn't work like GLFW key press callback function
	// where there is additional GLFW_REPEAT and GLFW_PRESS is treated
	// as a single press. Here, GLFW_PRESS updates every frame.
	if (glfwGetKey(window, GLFW_KEY_W)) {
		camera_move( g_camera, camera_forward * speed );
	}

	if (glfwGetKey(window, GLFW_KEY_S)) {
		camera_move( g_camera, -camera_forward * speed );
	}

	if (glfwGetKey(window, GLFW_KEY_A)) {
		camera_move( g_camera, -camera_right * speed );
	}

	if (glfwGetKey(window, GLFW_KEY_D)) {
		camera_move( g_camera, camera_right * speed );
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE)) {
		camera_move( g_camera, camera_up * speed );
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		camera_move( g_camera, -camera_up * speed );
	}
}

void single_press_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS) {
		imgui_draw_edit_window = !imgui_draw_edit_window;
		StringView_ASCII mode_name = ( imgui_draw_edit_window ) ? "SHOW" : "HIDE";
		log_info( "'Edit' window mode: " StringViewFormat ".",
			StringViewArgument( mode_name )
		);
	}

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		mouse.cursor_mode = !mouse.cursor_mode;

		if (mouse.cursor_mode) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		} else {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		StringView_ASCII mode_name = ( mouse.cursor_mode ) ? "ON" : "OFF";
		log_info( "Cursor mode: " StringViewFormat ".",
			StringViewArgument( mode_name )
		);
	}
}

void glfw_error_callback(int error_code, const char* description) {
	log( LogLevel_Error, "GLFW", "" StringViewFormat " (Error code: %d).",
		StringViewArgument( string_view( description ) ),
		error_code
	);
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
	console_init( CP_UTF8 );
	log_init();

	GLFWwindow* window;

	glfwSetErrorCallback(glfw_error_callback);

	if ( glfwInit() ) {
		log_info( "GLFW initialized." );
	} else {
		log_error( "Failed to initialize GLFW!" );
		Assert( false );
		exit( EXIT_FAILURE );
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
	if ( window ) {
		StringView_ASCII mode_name = ( monitor ) ? "Fullscreen" : "Windowed";
		log_info( "GLFW Window created (%dx%d, " StringViewFormat ").",
			screen.width,
			screen.height,
			StringViewArgument( mode_name )
		);
	} else {
		log_error( "Failed to open GLFW window!" );
		Assert( false );
		exit( EXIT_FAILURE );
	}

	glfwMakeContextCurrent(window);

	if ( glewInit() == GLEW_OK ) {
		log_info( "GLEW initialized." );
	} else {
		log_error( "Failed to initialize GLEW!");
		Assert( false );
		exit( EXIT_FAILURE );
	}

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

	textures_init();
	materials_init();
	models_init();
	renderer_init();
	maps_init();

	load_textures();
	load_shaders();
	create_materials();
	cameras_init();

	map_change( "empty" );

	Model_ID model_cube_id = model_load_from_file( "cube", "resources/models/cube.obj" );
	renderer_model_meshes_upload( model_cube_id );
	Model *model_cube = model_instance( model_cube_id );
	Mesh_ID cube_mesh_id = model_cube->meshes.data[ 0 ];
	Mesh *cube_mesh = mesh_instance( cube_mesh_id );
	cube_mesh->material_id = material_find( "rocks-medium" );

	Map *map = map_current();

	g_camera = camera_create(
		/*       name */ "Main Camera",
		/* projection */ CameraProjection_Perspective,
		/*   position */ Vector3_f32( 0.0f, 0.0f, -3.0f ),
		/*   rotation */ quaternion_identity(),
		/*        fov */ 80.0f,
		/*     z_near */ 0.01f,
		/*      z_far */ 2000.0f,
		/*   viewport */ Vector2_f32( 1280.0f, 720.0f )
	);

	glViewport(
		0,
		0,
		( GLsizei ) g_camera->viewport.width,
		( GLsizei ) g_camera->viewport.height
	);

	renderer_set_view_matrix_pointer( &g_camera->view_matrix );
	renderer_set_projection_matrix_pointer( &g_camera->projection_matrix );

	log_info(
		"GPU Vendor: " StringViewFormat ", GPU Name: " StringViewFormat,
		StringViewArgument( renderer_device_vendor() ),
		StringViewArgument( renderer_device_name() )
	);

	// Here we enable 'Z-buffer' to allow us render
	// 3D objects without overwriting already rendered pixels.
	glEnable(GL_DEPTH_TEST);

	//
	// --- ImGui ---
	//
	IMGUI_CHECKVERSION();
	auto imgui_context = ImGui::CreateContext();
	if (imgui_context) {
		log_info( "ImGui context created." );
	} else {
		log_error( "Failed to create ImGui context!" );
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

	while (!glfwWindowShouldClose(window))
	{
		process_input(window);

		cameras_update();

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


				ImGui::TextUnformatted("CAMERA:");

				ImGui::InputFloat3("Position", &g_camera->position[ 0 ]);
				ImGui::InputFloat4("Rotation (Quaternion)", &g_camera->rotation[ 0 ]);
				ImGui::DragFloat("Field of View", &g_camera->fov, 1.0f, 1.0f, 179.0f);
				ImGui::InputFloat("Clip Distance Near", &g_camera->z_near);
				ImGui::InputFloat("Clip Distance Far", &g_camera->z_far);
				ImGui::InputFloat2("Viewport Dimensions", &g_camera->viewport[ 0 ]);
				// ImGui::ColorEdit3("Clear color", &);
				ImGui::Checkbox("Freeze", &freeze_camera);

				/*
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

				*/
				ImGui::Separator();
				ImGui::TextUnformatted("RENDERER:");

				f32 frame_time = renderer_frame_time_delta();

				ImGui::Text("GPU Vendor: " StringViewFormat, StringViewArgument( renderer_device_vendor() ));
				ImGui::Text("GPU Name: " StringViewFormat, StringViewArgument( renderer_device_name() ));
				ImGui::Text("ImGui: Frametime: %.3f ms/frame (%.1f FPS)", 1000.0f / imgui_io.Framerate, imgui_io.Framerate);
				ImGui::Text("Renderer: Frametime: %.3f ms/frame (%.1f FPS)", frame_time, 1000.0f / frame_time);

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