#ifndef QLIGHT_MATERIAL_H
#define QLIGHT_MATERIAL_H

#include "common.h"
#include "texture.h"
#include "string.h"

// #include "renderer.h"
struct Renderer_Shader_Program;

struct Material {
	StringView_ASCII name;
	Renderer_Shader_Program *shader_program;
	// Vector4_f32 base_color
	Texture_ID diffuse;
	Texture_ID normal_map;
	Texture_ID specular_map;
	// metallic_roughness
	f32 shininess_exponent;
};

typedef u16 Material_ID;
constexpr Material_ID INVALID_MATERIAL_ID = U16_MAX;

bool materials_init();
void materials_shutdown();

Material_ID material_create(
	StringView_ASCII name,
	Renderer_Shader_Program *shader_program,
	Texture_ID diffuse,
	Texture_ID normal_map,
	Texture_ID specular_map,
	f32 shininess_exponent = 0.0f
);
bool material_destroy( Material_ID material_id );

Material_ID material_find( StringView_ASCII name );
Material * material_instance( Material_ID material_id );

#endif /* QLIGHT_MATERIAL_H */
