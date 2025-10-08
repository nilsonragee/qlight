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
bool imgui_draw_entities_window = true;
bool imgui_draw_materials_window = true;
bool freeze_light_change = false;
bool freeze_camera = false;

Vector2_f32 g_cursor_position;

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
	// In 'Cursor mode' we don't move camera.
	f32 offset_x = ( f32 )new_mouse_x - mouse.x;
	f32 offset_y = mouse.y - ( f32 )new_mouse_y; // Reversed, since  y-coordinates go from bottom to top.

	mouse.x = ( f32 )new_mouse_x;
	mouse.y = ( f32 )new_mouse_y;

	if (!mouse.cursor_mode) {
		offset_x *= mouse.sensitivity;
		offset_y *= mouse.sensitivity;
		Quaternion delta_pitch = quaternion_from_axis_angle( WORLD_DIRECTION_RIGHT, radians( -offset_y ) );
		Quaternion delta_yaw = quaternion_from_axis_angle( WORLD_DIRECTION_UP, radians( offset_x ) );
		// Quaternion *q;
		int i = g_event_index;
		// q = &delta_pitch;
		// log_info( "[%d] delta_pitch: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		// q = &delta_yaw;
		// log_info( "[%d] delta_yaw: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		// q = &g_camera->rotation;
		// log_info( "[%d] (before) camera->rotation: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
		// Vector4_f32 *v;
		// v = &g_camera->view_matrix[ 0 ];
		// log_info( "[%d] (before) camera->view[0]: [% .3f, % .3f, % .3f, % .3f]", i, v->x, v->y, v->z, v->w );
		// v = &g_camera->view_matrix[ 1 ];
		// log_info( "[%d] (before) camera->view[1]: [% .3f, % .3f, % .3f, % .3f]", i, v->x, v->y, v->z, v->w );
		// v = &g_camera->view_matrix[ 2 ];
		// log_info( "[%d] (before) camera->view[2]: [% .3f, % .3f, % .3f, % .3f]", i, v->x, v->y, v->z, v->w );
		// v = &g_camera->view_matrix[ 3 ];
		// log_info( "[%d] (before) camera->view[3]: [% .3f, % .3f, % .3f, % .3f]", i, v->x, v->y, v->z, v->w );
		camera_rotate_by_quaternion_global_space( g_camera, delta_yaw );
		camera_rotate_by_quaternion_local_space( g_camera, delta_pitch );
		// g_camera->rotation = quaternion_multiply( g_camera->rotation, delta_yaw );
		// g_camera->rotation = quaternion_multiply( delta_pitch, g_camera->rotation );
		// g_camera->rotation = quaternion_multiply( g_camera->rotation, delta_yaw );
		// g_camera->rotation = quaternion_multiply( delta_pitch, g_camera->rotation );
		// camera_reconstruct_rotation( g_camera );
		//g_camera->bits |= CameraBit_NeedsViewUpdate;
		// log_info( "[%d] (after) camera->rotation: (%.3f, %.3f, %.3f, %.3f)", i, q->x, q->y, q->z, q->w );
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
		// camera_move( g_camera, camera_up * speed );
		Vector3_f32 world_up = WORLD_DIRECTION_UP;
		camera_move( g_camera, world_up * speed );
	}

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT)) {
		// camera_move( g_camera, -camera_up * speed );
		Vector3_f32 world_up = WORLD_DIRECTION_UP;
		camera_move( g_camera, -world_up * speed );
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
			glfwSetCursorPos(window, g_cursor_position.x, g_cursor_position.y);
		} else {
			g_cursor_position.x = mouse.x;
			g_cursor_position.y = mouse.y;
			mouse.x = screen.width / 2.0f;
			mouse.y = screen.height / 2.0f;
			glfwSetCursorPos(window, mouse.x, mouse.y);
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
		.bits = RendererVertexAttributeBit_Active
	} );

	/* Uniforms */

	Array< Renderer_Uniform > *phong_uniforms = &shader->uniforms;

	/* Vertex stage uniforms */

	/* Fragment stage uniforms */

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "gbuffer_position",
		.data_type = RendererDataType_s32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "gbuffer_normal",
		.data_type = RendererDataType_s32,
	} );

	array_add( phong_uniforms, Renderer_Uniform {
		.name = "gbuffer_diffuse_specular",
		.data_type = RendererDataType_s32,
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
		.name = "shininess_exponent",
		.data_type = RendererDataType_f32,
	} );

	renderer_shader_program_update_uniform_locations( shader );

	/* Uniform Buffers */

	Array< Renderer_Uniform_Buffer > *phong_uniform_buffers = &shader->uniform_buffers;

	// Here we reconstruct shader-bound declaration info.
	// Ideally, this should be done by some shader parser.
	array_add( phong_uniform_buffers, Renderer_Uniform_Buffer {
		.name = "Lights",
		.size = sizeof( Uniform_Buffer_Lights ),
		.binding = 0
		// .storage_bits
		// .opengl_ubo
	} );
}

void load_shaders() {
	load_phong_lighting_shader();
}

void load_texture( StringView_ASCII name, StringView_ASCII file_path, Texture_Channels channels, GLint opengl_storage_format ) {
	Texture_ID texture_id = texture_load_from_file( name, file_path, channels );
	Texture *texture = texture_instance( texture_id );
	renderer_texture_2d_upload(
		/*            texture_id */ texture_id,
		/*                origin */ { 0, 0 },
		/*            dimensions */ texture->dimensions,
		/*         mipmap_levels */ 5,
		/* opengl_storage_format */ opengl_storage_format,
		/*     opengl_pixel_type */ GL_UNSIGNED_BYTE
	);
}

void load_textures() {
//#ifdef LOAD_TEXTURES
	load_texture( "bark_diffuse", "resources/textures/bark_diffuse_x3072_expt1-255.png", TextureChannels_RGB, GL_SRGB8 );
	load_texture( "bark_normal", "resources/textures/bark_normal_x3072_expt1-255_gauss-bilat.png", TextureChannels_RGB, GL_RGB8 );
#ifdef LOAD_TEXTURES

	load_texture( "dried-soil_diffuse", "resources/textures/dried_soil_diffuse_x3072_expt1-190.png", TextureChannels_RGB, GL_RGB8 );
	load_texture( "dried-soil_normal", "resources/textures/dried_soil_normal_x3072_expt1-190.png", TextureChannels_RGB, GL_RGB8 );

	load_texture( "rocks-medium_diffuse", "resources/textures/rocks-medium_diffuse_x3072_expt1-394_flat.png", TextureChannels_RGB, GL_RGB8 );
	load_texture( "rocks-medium_normal", "resources/textures/rocks-medium_normal_x3072_expt1-394_flat.png", TextureChannels_RGB, GL_RGB8 );
#endif
}

void create_materials() {
	Renderer_Shader_Program *shader = renderer_find_shader_program( "phong_program" );
	Assert( shader );
	Texture_ID diffuse = INVALID_TEXTURE_ID;
	Texture_ID normal = INVALID_TEXTURE_ID;
	Texture_ID specular = INVALID_TEXTURE_ID;
	float shininess_exponent = 0.0f;

	material_create( "dummy", NULL, INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, INVALID_TEXTURE_ID, 0.0f );

//#ifdef LOAD_TEXTURES
	diffuse = texture_find( "bark_diffuse" );
	normal = texture_find( "bark_normal" );
	specular = INVALID_TEXTURE_ID;
	Assert( diffuse != INVALID_TEXTURE_ID );
	Assert( normal != INVALID_TEXTURE_ID );
//#endif
	shininess_exponent = 64.0f;
	material_create( "bark", shader, diffuse, normal, specular, shininess_exponent );

//#endif
#ifdef LOAD_TEXTURES
	diffuse = texture_find( "dried-soil_diffuse" );
	normal = texture_find( "dried-soil_normal" );
	specular = INVALID_TEXTURE_ID;
	Assert( diffuse != INVALID_TEXTURE_ID );
	Assert( normal != INVALID_TEXTURE_ID );
#endif
	shininess_exponent = 0.0f;
	material_create( "dried-soil", shader, diffuse, normal, specular, shininess_exponent );

#ifdef LOAD_TEXTURES
	diffuse = texture_find( "rocks-medium_diffuse" );
	normal = texture_find( "rocks-medium_normal" );
	specular = INVALID_TEXTURE_ID;
	Assert( diffuse != INVALID_TEXTURE_ID );
	Assert( normal != INVALID_TEXTURE_ID );
#endif
	shininess_exponent = 0.0f;
	material_create( "rocks-medium", shader, diffuse, normal, specular, shininess_exponent );
}

// Not sure whether this is the right place...
// The `Entity` header does not have any functions as well as a source file.
// Maybe it would be moved there when there would be one.
static StringView_ASCII
entity_type_name( Entity_Type type ) {
	switch ( type ) {
		case EntityType_Player: return "Player";
		case EntityType_Camera: return "Camera";
		case EntityType_StaticObject: return "StaticObject";
		case EntityType_DynamicObject: return "DynamicObject";

		case EntityType_DirectionalLight: return "DirectionalLight";
		case EntityType_PointLight: return "PointLight";
		case EntityType_SpotLight: return "SpotLight";

		case EntityType_COUNT:
		case EntityType_None:
		default: return ( const char * )NULL;
	}
}

static bool
imgui_entity_base_fields( Entity *entity ) {
	ImGui::TextDisabled( "--- Entity ---" );
	bool modified = false;

	ImGui::InputScalar( "Parent ID", ImGuiDataType_U16, &entity->parent );
	// ImGui::Text( "Parent: %hu", entity->parent );

	if ( ImGui::TreeNode( "Bits" ) ) {
		modified |= ImGui::CheckboxFlags( "NoDraw", ( u32 * )&entity->bits, EntityBit_NoDraw );

		ImGui::TreePop();
	}

	if ( ImGui::TreeNode( "Transform" ) ) {
		bool modified_transform = false;
		Transform *t = &entity->transform;
		modified_transform |= ImGui::DragFloat3("Position", &t->position[ 0 ], 0.01f, -F32_MAX, F32_MAX, "%.1f", ImGuiSliderFlags_NoRoundToFormat );
		modified_transform |= ImGui::DragFloat4("Rotation (Quaternion)", &t->rotation[ 0 ], 0.001f, -1.0f, 1.0f, "%.3f" );
		modified_transform |= ImGui::DragFloat3("Scale", &t->scale[ 0 ], 0.01f, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat );
		modified |= modified_transform;

		if ( modified_transform )
			transform_recalculate_matrices( t );

		ImGui::TreePop();
	}

	return modified;
}

static bool
imgui_entity_player_fields( Entity_Player *player ) {
	ImGui::TextDisabled( "--- Entity_Player ---" );
	ImGui::Text( "Name: " StringViewFormat, StringViewArgument( player->name ) );
	return false;
}

static bool
imgui_camera_fields( Camera *camera ) {
	return false;
}

static bool
imgui_model_fields( Model *camera ) {
	return false;
}

static bool
imgui_entity_camera_fields( Entity_Camera *entity ) {
	ImGui::TextDisabled( "--- Entity_Camera ---" );
	bool modified = false;

	ImGui::Text( "Name: " StringViewFormat, StringViewArgument( entity->name ) );
	Camera *camera = entity->camera;
	StringView_ASCII name = ( camera ) ? camera->name : string_view( ( const char * )NULL );
	if ( ImGui::TreeNode( "Camera", "Camera Component: 0x%llX ('" StringViewFormat "')",
		camera,
		StringViewArgument( name )
	) ) {
		if ( camera )
			modified |= imgui_camera_fields( camera );

		ImGui::TreePop();
	}
	return modified;
}

static bool
imgui_entity_static_object_fields( Entity_Static_Object *object ) {
	ImGui::TextDisabled( "--- Entity_Static_Object ---" );
	bool modified = false;
	modified |= ImGui::InputScalar( "Model ID", ImGuiDataType_U16, &object->model );
	return modified;
}

static bool
imgui_entity_dynamic_object_fields( Entity_Dynamic_Object *object ) {
	ImGui::TextDisabled( "--- Entity_Dynamic_Object ---" );
	bool modified = false;
	modified |= ImGui::InputScalar( "Model ID", ImGuiDataType_U16, &object->model );
	return modified;
}

static bool
imgui_entity_directional_light_fields( Entity_Directional_Light *light ) {
	ImGui::TextDisabled( "--- Entity_Directional_Light ---" );
	bool modified = false;
	modified |= ImGui::ColorEdit3( "Color", &light->color[ 0 ] );
	modified |= ImGui::DragFloat( "Intensity", &light->intensity, 0.01f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_NoRoundToFormat);
	return modified;
}

static bool
imgui_entity_point_light_fields( Entity_Point_Light *light ) {
	ImGui::TextDisabled( "--- Entity_Point_Light ---" );
	bool modified = false;
	modified |= ImGui::ColorEdit3( "Color", &light->color[ 0 ] );
	modified |= ImGui::DragFloat( "Intensity", &light->intensity, 0.01f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_NoRoundToFormat);
	return modified;
}

static bool
imgui_entity_spot_light_fields( Entity_Spot_Light *light ) {
	ImGui::TextDisabled( "--- Entity_Spot_Light ---" );
	bool modified = false;
	modified |= ImGui::ColorEdit3( "Color", &light->color[ 0 ] );
	modified |= ImGui::DragFloat( "Intensity", &light->intensity, 0.01f, 0.0f, 100.0f, "%.2f", ImGuiSliderFlags_NoRoundToFormat);
	return modified;
}

static bool
imgui_entity_derived_fields( Entity *entity ) {
	switch ( entity->type ) {
		case EntityType_Player:  return imgui_entity_player_fields( ( Entity_Player * )entity );
		case EntityType_Camera:  return imgui_entity_camera_fields( ( Entity_Camera * )entity );
		case EntityType_StaticObject:  return imgui_entity_static_object_fields( ( Entity_Static_Object * )entity );
		case EntityType_DynamicObject:  return imgui_entity_dynamic_object_fields( ( Entity_Dynamic_Object * )entity );

		case EntityType_DirectionalLight:  return imgui_entity_directional_light_fields( ( Entity_Directional_Light * )entity );
		case EntityType_PointLight:  return imgui_entity_point_light_fields( ( Entity_Point_Light * )entity );
		case EntityType_SpotLight:  return imgui_entity_spot_light_fields( ( Entity_Spot_Light * )entity );

		case EntityType_None:
		default:
			AssertMessage( false, "Encountered 'None' or unhandled type of Entity while displaying entity fields in ImGui" );
			return false;
	}
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

	mouse.x = screen.width / 2.0f;
	mouse.y = screen.height / 2.0f;
	g_cursor_position.x = mouse.x;
	g_cursor_position.y = mouse.y;

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

	// map_change( "empty" ); // happens in `maps_init()`

	Model_ID model_cube_id = model_load_from_file( "cube", "resources/models/cube.obj" );
	renderer_model_meshes_upload( model_cube_id );
	Model *model_cube = model_instance( model_cube_id );
	Mesh_ID cube_mesh_id = model_cube->meshes.data[ 0 ];
	Mesh *cube_mesh = mesh_instance( cube_mesh_id );
	cube_mesh->material_id = material_find( "bark" );

	Map *map = map_current();

	g_camera = camera_create(
		/*       name */ "Main Camera",
		/* projection */ CameraProjection_Perspective,
		// /*   position */ { 0.0f, 0.0f, -3.0f },
		/*   position */ { 3.1f, 2.0f, 2.7f },
		// /*   rotation */ quaternion_identity(),
		/*   rotation */ Quaternion( -0.102f, 0.882f, -0.226f, -0.401f ),
		/* ortho_size */ 5.0f,
		// /*        fov */ 80.0f,
		/*        fov */ 40.0f,
		/*     z_near */ 0.1f,
		/*      z_far */ 2000.0f,
		/*   viewport */ { 1280, 720 }
	);

	renderer_set_camera_position_pointer( &g_camera->position );

	// Add some entities
	{
		Entity_Directional_Light entity_sunlight;
		// Entity
		entity_sunlight.type = EntityType_DirectionalLight;
		entity_sunlight.parent = INVALID_ENTITY_ID;
		entity_sunlight.bits = EntityBit_NoDraw;
		entity_sunlight.transform = transform_identity();
		entity_sunlight.transform.position.x = 100.0f;
		entity_sunlight.transform.position.y = 100.0f;
		entity_sunlight.transform.position.z = 10.0f;
		// Entity_Directional_Light
		entity_sunlight.color = { 244 / 255.0f, 233 / 255.0f, 155 / 255.0f };
		entity_sunlight.intensity = 1.0f;
		map_entity_add( map, &entity_sunlight );

		Entity_Point_Light entity_point_light;
		// Entity
		entity_point_light.type = EntityType_PointLight;
		entity_point_light.parent = INVALID_ENTITY_ID;
		entity_point_light.bits = EntityBit_NoDraw;
		entity_point_light.transform = transform_identity();
		entity_point_light.transform.position.y = 10.0f;
		// Entity_Point_Light
		entity_point_light.color = { 0.0f, 0.0f, 1.0f };
		entity_point_light.intensity = 1.0f;
		map_entity_add( map, &entity_point_light );

		Entity_Camera entity_camera;
		// Entity
		entity_camera.type = EntityType_Camera;
		entity_camera.parent = INVALID_ENTITY_ID;
		entity_camera.bits = EntityBit_NoDraw;
		entity_camera.transform = transform_identity();
		// Entity_Camera
		entity_camera.name = string_new( sys_allocator, "Main Camera" );
		entity_camera.camera = g_camera;
		Entity_ID entity_camera_id = map_entity_add( map, &entity_camera );

		Entity_Camera entity_player;
		// Entity
		entity_player.type = EntityType_Player;
		entity_player.parent = entity_camera_id;
		entity_player.bits = 0;
		entity_player.transform = transform_identity();
		// Entity_Player
		entity_player.name = string_new( sys_allocator, "Player" );;
		map_entity_add( map, &entity_player );

		Entity_Static_Object entity_cube;
		// Entity
		entity_cube.type = EntityType_StaticObject;
		entity_cube.parent = INVALID_ENTITY_ID;
		entity_cube.bits = 0;
		entity_cube.transform = transform_identity();
		// Entity_Static_Object
		entity_cube.model = model_cube_id;
		// entity_cube.transform.position += { 1, 0, 0 };
		// transform_set_dirty( &entity_cube.transform, true );
		map_entity_add( map, &entity_cube );
	}

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

		// renderer_queue_draw_command(
		// 	/*     mesh_id */ cube_mesh_id,
		// 	/* material_id */ cube_mesh->material_id,
		// 	/*   transform */ &model_cube->transform
		// );

		// Update Uniform_Buffer_Lights
		maps_update_lights_manager();

		// Draw entities
		ForIt( map->entity_table.entities.data, map->entity_table.entities.size ) {
			if ( it == NULL )
				continue;

			if ( it->bits & EntityBit_NoDraw )
				continue;

			if ( it->type == EntityType_StaticObject ) {
				Entity_Static_Object *static_object = ( Entity_Static_Object * )it;
				Model *model = model_instance( static_object->model );
				Mesh *mesh = mesh_instance( model->meshes.data[ 0 ] );
				transform_recalculate_dirty_matrices( &static_object->transform );
				Transform_XYZW *transform_xyzw = ( Transform_XYZW * )&static_object->transform;
				renderer_queue_draw_command(
					/*     mesh_id */ model->meshes.data[ 0 ],
					/* material_id */ mesh->material_id,
					/*   transform */ &static_object->transform
				);
			}
		}}

		renderer_draw_frame();

		//
		// --- ImGui Render ---
		//
		glBindFramebuffer( GL_FRAMEBUFFER, 0 );
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static int g_frame_idx = 1;

		if (imgui_draw_demo_window) {
			ImGui::ShowDemoWindow(&imgui_draw_demo_window);
		}

		if (imgui_draw_edit_window) {

			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowCollapsed(false, ImGuiCond_FirstUseEver);
			if (ImGui::Begin("Scene")) {
				ImGui::Checkbox("Show demo window", &imgui_draw_demo_window);

				ImGui::TextUnformatted("CAMERA:");

				bool update_view = false;
				bool update_projection = false;
				update_view |= ImGui::DragFloat3("Position", &g_camera->position[ 0 ], 0.5f, -1000.0f, 1000.0f, "%.1f", ImGuiSliderFlags_Logarithmic | ImGuiSliderFlags_NoRoundToFormat);
				update_view |= ImGui::DragFloat4("Rotation (Quaternion)", &g_camera->rotation[ 0 ], 0.001f, -1.0f, 1.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
				update_projection |= ImGui::DragFloat("Orthographic Size (World units)", &g_camera->orthographic_size, 0.1f, 1.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
				update_projection |= ImGui::DragFloat("Field of View", &g_camera->fov, 1.0f, 1.0f, 179.0f, "%.1f");
				update_projection |= ImGui::DragFloat("Clip Distance Near", &g_camera->z_near, 0.01f, 0.01f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
				update_projection |= ImGui::DragFloat("Clip Distance Far", &g_camera->z_far, 1.0f, 100.0f, 5000.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
				update_projection |= ImGui::DragFloat2("Viewport Dimensions", &g_camera->viewport[ 0 ], 10.0f, 100.0f, 8192.0f);
				bool is_orthographic = g_camera->bits & CameraBit_IsOrthographic;
				StringView_ASCII projection_name = ( is_orthographic ) ? "Orthographic" : "Perspective";
				bool change_perspective = ImGui::Button( projection_name.data );
				if ( change_perspective ) {
					if ( is_orthographic )  g_camera->bits &= ~CameraBit_IsOrthographic;
					else                    g_camera->bits |= CameraBit_IsOrthographic;
				}
				update_projection |= change_perspective;

				if ( update_view )        g_camera->bits |= CameraBit_NeedsViewUpdate;
				if ( update_projection )  g_camera->bits |= CameraBit_NeedsProjectionUpdate;

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

				static const char *g_render_outputs[] = { "Final Color", "Position", "Normal", "Diffuse & Specular" };
				static int g_selected_render_output = 0;
				if ( ImGui::Combo( "Render Output", &g_selected_render_output, g_render_outputs, ARRAY_SIZE( g_render_outputs ) ) ) {
					Renderer_Output_Channel output_channel = RendererOutputChannel_FinalColor;
					switch ( g_selected_render_output ) {
						case 0: output_channel = RendererOutputChannel_FinalColor; break;
						case 1: output_channel = RendererOutputChannel_Position; break;
						case 2: output_channel = RendererOutputChannel_Normal; break;
						case 3: output_channel = RendererOutputChannel_DiffuseSpecular; break;
					};
					renderer_set_output_channel( output_channel );
				}

				ImGui::Text("GPU Vendor: " StringViewFormat, StringViewArgument( renderer_device_vendor() ));
				ImGui::Text("GPU Name: " StringViewFormat, StringViewArgument( renderer_device_name() ));
				ImGui::Text("ImGui: Frametime: %.3f ms/frame (%.1f FPS)", 1000.0f / imgui_io.Framerate, imgui_io.Framerate);
				ImGui::Text("Renderer: Frametime: %.3f ms/frame (%.1f FPS)", frame_time, 1000.0f / frame_time);
			}

			if ( g_frame_idx == 2 ) {
				// The window's size is calculated right before the drawing, I think?
				// So a simple temporary solution is to let it calculate and draw the 1-st frame,
				//   and then get the actual size on the 2-nd frame.
				// The frame counter should be in Renderer, increased internally after every `renderer_draw_frame()`.
				ImVec2 window_pos = ImGui::GetWindowPos();
				ImVec2 window_size = ImGui::GetWindowSize();
				window_pos.x += window_size.x;
				ImGui::SetNextWindowPos( window_pos, ImGuiCond_FirstUseEver );
			}

			ImGui::End();
		}

		if ( imgui_draw_entities_window ) {
			if ( ImGui::Begin( "Entities", NULL, ImGuiWindowFlags_AlwaysAutoResize ) ) {
				ForIt( map->entity_table.entities.data, map->entity_table.entities.size ) {
					if ( it == NULL )
						continue;

					Entity_ID entity_id = map->entity_table.ids.data[ it_index ];
					StringView_ASCII type_name = entity_type_name( it->type );
					bool is_light_entity = entity_type_is_light_source( it->type );
					if ( ImGui::TreeNode( (void*)(intptr_t)it_index, "%hu: " StringViewFormat, entity_id, StringViewArgument( type_name ) ) ) {
						bool modified = false;
						bool is_light_entity = entity_type_is_light_source( it->type );
						bool modified_base = imgui_entity_base_fields( it );
						bool modified_derived = imgui_entity_derived_fields( it );
						modified |= modified_base;
						modified |= modified_derived;
						if ( modified && is_light_entity )
							map_entity_light_update( map, it->type, it );

						if ( ImGui::Button( "Delete" ) ) {
							map_entity_remove( map, entity_id );
						}

						ImGui::TreePop();
					}
				}}
			}

			if ( g_frame_idx == 2 ) {
				ImVec2 window_pos = ImGui::GetWindowPos();
				ImVec2 window_size = ImGui::GetWindowSize();
				window_pos.x += window_size.x;
				ImGui::SetNextWindowPos( window_pos, ImGuiCond_FirstUseEver );
			}

			ImGui::End();
		}

		if ( imgui_draw_materials_window ) {
			if ( ImGui::Begin( "Materials", NULL, ImGuiWindowFlags_AlwaysAutoResize ) ) {
				ArrayView< Material > materials = materials_get_storage_view();
				ForIt( materials.data, materials.size ) {
					if ( it.name.data == NULL )
						continue;

					Material_ID material_id = it_index;
					if ( ImGui::TreeNode( (void*)(intptr_t)it_index, "%hu: " StringViewFormat, material_id, StringViewArgument( it.name ) ) ) {
						StringView_ASCII shader_name = ( it.shader_program ) ? it.shader_program->name : string_view_empty();
						ImGui::Text( "Shader Program: 0x%X -> \"" StringViewFormat "\"", it.shader_program, StringViewArgument( shader_name ) );
						ImGui::InputScalar( "Diffuse Texture ID", ImGuiDataType_U16, &it.diffuse );
						ImGui::InputScalar( "Normal Texture ID", ImGuiDataType_U16, &it.normal_map );
						ImGui::InputScalar( "Specular Texture ID", ImGuiDataType_U16, &it.specular_map );
						ImGui::DragFloat( "Shininess Exponent", &it.shininess_exponent, 2.0f, 0.0f, 4096.0f, "%.1f" );

						ImGui::TreePop();
					}
				}}
			}

			ImGui::End();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
		glFlush();

		g_frame_idx += 1;
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	exit(EXIT_SUCCESS);
}