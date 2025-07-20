#ifndef QLIGHT_TRANSFORM_H
#define QLIGHT_TRANSFORM_H

#include "common.h"
#include "math.h"

#include "../libs/GLM/glm.hpp"
#include "../libs/GLM/gtc/matrix_transform.hpp"

typedef Vector4_f32 Quaternion;

struct Transform {
	Vector3_f32 position;
	Quaternion rotation;
	Vector3_f32 scale;

	// Cached matrices.
	// WARNING: Please do not use directly.
	// "Dirty" flag is contained in model_matrix[ 0 ][ 3 ] as INFINITY value. (1-st column, 4-th row)
	Matrix4x4_f32 model_matrix;
	Matrix3x3_f32 normal_matrix;
};

Quaternion quaternion_identity();
Transform transform_create();
Matrix4x4_f32 transform_model_matrix( Transform *transform );
Matrix3x3_f32 transform_normal_matrix( Matrix4x4_f32 *model_matrix );
void transform_recalculate_matrices( Transform *transform );
bool transform_recalculate_dirty_matrices( Transform *transform );

bool transform_is_dirty( Transform *transform );
void transform_set_dirty( Transform *transform );
void transform_clear_dirty( Transform *transform );

Matrix3x3_f32 quaternion_to_rotation_matrix( Quaternion q );

Matrix4x4_f32 transform_matrix_scale( Matrix4x4_f32 *model, Vector3_f32 scale );
Matrix4x4_f32 transform_matrix_rotate_matrix( Matrix4x4_f32 *model, Matrix3x3_f32 *rotation );
Matrix4x4_f32 transform_matrix_rotate_quaternion( Matrix4x4_f32 *model, Quaternion rotation );
Matrix4x4_f32 transform_matrix_translate( Matrix4x4_f32 *model, Vector3_f32 position );

// Right-handed, [-1, 1] depth range for the NDC (normalized device coordinates)
Matrix4x4_f32 projection_perspective( f32 fov_vertical_radians, f32 aspect_ratio, f32 z_near, f32 z_far );


// fpv_camera.view_matrix = glm::lookAt(fpv_camera.position, fpv_camera.position + fpv_camera.direction_front, fpv_camera.direction_up);
// glm::lookAt( Vector3 eye, Vector3 center, Vector3 up );
Matrix4x4_f32 projection_view( Vector3_f32 view_position, Vector3_f32 view_point, Vector3_f32 world_up );

#endif /* QLIGHT_TRANSFORM_H */