#ifndef QLIGHT_TRANSFORM_H
#define QLIGHT_TRANSFORM_H

#include "common.h"
#include "math.h"

#include "../libs/GLM/glm.hpp"
#include "../libs/GLM/gtc/matrix_transform.hpp"

typedef Vector4_f32 Quaternion;
typedef Vector4_f32_XYZW Quaternion_XYZW;

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

struct Transform_XYZW {
	Vector3_f32_XYZ position;
	Quaternion_XYZW rotation;
	Vector3_f32_XYZ scale;

	// Cached matrices.
	// WARNING: Please do not use directly.
	// "Dirty" flag is contained in model_matrix[ 0 ][ 3 ] as INFINITY value. (1-st column, 4-th row)
	Matrix4x4_f32_XYZW model_matrix;
	Matrix3x3_f32_XYZ normal_matrix;
};

Quaternion quaternion_identity();
Transform transform_identity();
Matrix4x4_f32 transform_model_matrix( Transform *transform );
Matrix3x3_f32 transform_normal_matrix( Matrix4x4_f32 *model_matrix );
void transform_recalculate_matrices( Transform *transform );
bool transform_recalculate_dirty_matrices( Transform *transform );

bool transform_is_dirty( Transform *transform );
void transform_set_dirty( Transform *transform, bool dirty );

Matrix3x3_f32 quaternion_to_rotation_matrix( Quaternion q );

Matrix4x4_f32 transform_matrix_scale( Matrix4x4_f32 *model, Vector3_f32 scale );
Matrix4x4_f32 transform_matrix_rotate_matrix( Matrix4x4_f32 *model, Matrix3x3_f32 *rotation );
Matrix4x4_f32 transform_matrix_rotate_quaternion( Matrix4x4_f32 *model, Quaternion rotation );
Matrix4x4_f32 transform_matrix_translate( Matrix4x4_f32 *model, Vector3_f32 position );

Quaternion yxz_euler_to_quaternion_rotation( Vector3_f32 yxz_euler );
inline Quaternion yxz_euler_degrees_to_quaternion_rotation( Vector3_f32 yxz_euler ) {
	yxz_euler.x = radians( yxz_euler.x );
	yxz_euler.y = radians( yxz_euler.y );
	yxz_euler.z = radians( yxz_euler.z );
	return yxz_euler_to_quaternion_rotation( yxz_euler );
}

Vector3_f32 quaternion_to_yxz_euler_rotation( Quaternion quaternion );
inline Vector3_f32 quaternion_to_yxz_euler_degrees_rotation( Quaternion quaternion ) {
	Vector3_f32 yxz_euler = quaternion_to_yxz_euler_rotation( quaternion );
	yxz_euler.x = degrees( yxz_euler.x );
	yxz_euler.y = degrees( yxz_euler.y );
	yxz_euler.z = degrees( yxz_euler.z );
	return yxz_euler;
}

Quaternion normalize( Quaternion q );

#endif /* QLIGHT_TRANSFORM_H */