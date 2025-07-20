#include "transform.h"

#include "../libs/GLM/glm.hpp"
#include "../libs/GLM/gtc/matrix_transform.hpp"

Quaternion quaternion_identity() {
	return Quaternion {
		.x = 0.0f,
		.y = 0.0f,
		.z = 0.0f,
		.w = 1.0f
	};
}

Transform transform_create() {
	Transform transform = {
		.position = Vector3_f32 {
			.x = 0.0f,
			.y = 0.0f,
			.z = 0.0f
		},
		.rotation = quaternion_identity(),
		.scale = Vector3_f32 {
			.x = 1.0f,
			.y = 1.0f,
			.z = 1.0f
		}
	};

	transform_set_dirty( &transform );
	return transform;
}

static void place_matrix3x3_into_matrix4x4( Matrix3x3_f32 *a, Matrix4x4_f32 *b ) {
	(*b)[ 0 ][ 0 ] = (*a)[ 0 ][ 0 ];
	(*b)[ 0 ][ 1 ] = (*a)[ 0 ][ 1 ];
	(*b)[ 0 ][ 2 ] = (*a)[ 0 ][ 2 ];

	(*b)[ 1 ][ 0 ] = (*a)[ 1 ][ 0 ];
	(*b)[ 1 ][ 1 ] = (*a)[ 1 ][ 1 ];
	(*b)[ 1 ][ 2 ] = (*a)[ 1 ][ 2 ];

	(*b)[ 2 ][ 0 ] = (*a)[ 2 ][ 0 ];
	(*b)[ 2 ][ 1 ] = (*a)[ 2 ][ 1 ];
	(*b)[ 2 ][ 2 ] = (*a)[ 2 ][ 2 ];
}

void transform_recalculate_matrices( Transform *transform ) {
	Matrix3x3_f32 rotation = quaternion_to_rotation_matrix( transform->rotation ); // Vector4 -> Matrix3x3
	// Turn scale to Matrix3x3 ?
	Matrix3x3_f32 rotation_scale = rotation;
	rotation_scale[ 0 ] *= transform->scale.x;
	rotation_scale[ 1 ] *= transform->scale.y;
	rotation_scale[ 2 ] *= transform->scale.z;

	/*
	Matrix4x4_f32 model;
	place_matrix3x3_into_matrix4x4( &rotation_scale, &model );
	transform_matrix_translate( &model, transform->position );
	*/

	place_matrix3x3_into_matrix4x4( &rotation_scale, &transform->model_matrix );
	transform_matrix_translate( &transform->model_matrix, transform->position );

	/*
	Matrix3x3_f32 normal_matrix;
	bool non_uniform_scaling =
		( transform->scale.y != transform->scale.x ) ||
		( transform->scale.z != transform->scale.x );
	if ( non_uniform_scaling ) {
		normal_matrix = matrix3x3_f32_inverse( rotation );
		normal_matrix = matrix3x3_f32_transpose( normal_matrix );
	} else {
		normal_matrix = rotation;
	}
	*/

	bool non_uniform_scaling =
		( transform->scale.y != transform->scale.x ) ||
		( transform->scale.z != transform->scale.x );
	if ( non_uniform_scaling ) {
		transform->normal_matrix = matrix3x3_f32_inverse( rotation );
		transform->normal_matrix = matrix3x3_f32_transpose( transform->normal_matrix );
	} else {
		transform->normal_matrix = rotation;
	}

	// model_matrix[ 0 ][ 3 ]  INFINITY -> 0.0
	transform_clear_dirty( transform );

	/*
	model = transform_matrix_scale( &model, transform->scale );
	model = transform_matrix_rotate_quaternion( &model, transform->rotation );
	model = transform_matrix_translate( &model, transform->position );
	*/
}

bool transform_recalculate_dirty_matrices( Transform *transform ) {
	if ( !transform_is_dirty( transform ) )
		return false;

	transform_recalculate_matrices( transform );
	// transform->model_matrix = transform_model_matrix( transform );
	// transform->normal_matrix = transform_normal_matrix( &transform->model_matrix );
	return true;
}

bool transform_is_dirty( Transform *transform ) {
	return ( transform->model_matrix[ 0 ][ 3 ] == INFINITY );
}

void transform_set_dirty( Transform *transform ) {
	transform->model_matrix[ 0 ][ 3 ] = INFINITY;
}

void transform_clear_dirty( Transform *transform ) {
	transform->model_matrix[ 0 ][ 3 ] = 0.0f;
}

Matrix3x3_f32 quaternion_to_rotation_matrix( Quaternion q ) {
	Matrix3x3_f32 result;

	f32 x = q.x;
	f32 y = q.y;
	f32 z = q.z;
	f32 w = q.w;

	result[ 0 ] = {
		1  -  2 * y * y  -  2 * z * z,
		2 * x * y  +  2 * w * z,
		2 * x * z  -  2 * w * y
	};
	result[ 1 ] = {
		2 * x * y  -  2 * w * z,
		1  -  2 * x * x  -  2 * z * z,
		2 * y * z  +  2 * w * x
	};
	result[ 2 ] = {
		2 * x * z  +  2 * w * y,
		2 * y * z  -  2 * w * x,
		1  -  2 * x * x  -  2 * y * y
	};

	return result;
}

Matrix4x4_f32 transform_matrix_scale( Matrix4x4_f32 *model, Vector3_f32 scale ) {
	Matrix4x4_f32 result;

	result[ 0 ] = (*model)[ 0 ] * scale.x;
	result[ 1 ] = (*model)[ 1 ] * scale.y;
	result[ 2 ] = (*model)[ 2 ] * scale.z;
	result[ 3 ] = (*model)[ 3 ];

	return result;
}

Matrix4x4_f32 transform_matrix_rotate_matrix( Matrix4x4_f32 *model, Matrix3x3_f32 *rotation ) {
	Matrix4x4_f32 result = *model;
	for ( uint32_t i = 0; i < 3; i += 1 ) {
		f32 temp_0 = result[ i ][ 0 ];
		f32 temp_1 = result[ i ][ 1 ];
		f32 temp_2 = result[ i ][ 2 ];

		result[ i ][ 0 ] = temp_0 * (*rotation)[ 0 ][ 0 ]  +  temp_1 * (*rotation)[ 1 ][ 0 ]  + temp_2 * (*rotation)[ 2 ][ 0 ];
		result[ i ][ 1 ] = temp_0 * (*rotation)[ 0 ][ 1 ]  +  temp_1 * (*rotation)[ 1 ][ 1 ]  + temp_2 * (*rotation)[ 2 ][ 1 ];
		result[ i ][ 2 ] = temp_0 * (*rotation)[ 0 ][ 2 ]  +  temp_1 * (*rotation)[ 1 ][ 2 ]  + temp_2 * (*rotation)[ 2 ][ 2 ];
	}

	return result;
}

Matrix4x4_f32 transform_matrix_rotate_quaternion( Matrix4x4_f32 *model, Quaternion rotation ) {
	Matrix3x3_f32 rotation_matrix = quaternion_to_rotation_matrix( rotation );
	Matrix4x4_f32 result = transform_matrix_rotate_matrix( model, &rotation_matrix );
	return result;
}

Matrix4x4_f32 transform_matrix_translate( Matrix4x4_f32 *model, Vector3_f32 position ) {
	Matrix4x4_f32 result;

	Vector4_f32 direction_X = (*model)[ 0 ];
	Vector4_f32 direction_Y = (*model)[ 1 ];
	Vector4_f32 direction_Z = (*model)[ 2 ];
	Vector4_f32 model_position = (*model)[ 3 ];

	result[ 0 ] = direction_X;
	result[ 1 ] = direction_Y;
	result[ 2 ] = direction_Z;

	result[ 3 ] =
		direction_X * position.x  +
		direction_Y * position.y  +
		direction_Z * position.z  +
		model_position;

	return result;
}

Matrix4x4_f32 projection_perspective( f32 fov_vertical_radians, f32 aspect_ratio, f32 z_near, f32 z_far ) {
	// Right-handed, [-1, 1] depth range for the NDC (normalized device coordinates)
	Matrix4x4_f32 result = Matrix4x4_f32( 0.0f );

	f32 tan_half_fov = tan( fov_vertical_radians / 2.0f );

	result[ 0 ][ 0 ] = 1.0f / ( aspect_ratio * tan_half_fov );
	result[ 1 ][ 1 ] = 1.0f / tan_half_fov;
	result[ 2 ][ 2 ] = - ( z_far + z_near ) / ( z_far - z_near );
	result[ 2 ][ 3 ] = - 1.0f;
	result[ 3 ][ 2 ] = - ( 2.0f * z_far * z_near ) / ( z_far - z_near );

	return result;
}

Matrix4x4_f32 projection_view( Vector3_f32 view_position, Vector3_f32 view_point, Vector3_f32 world_up ) {
	// Right-handed
	Vector3_f32 forward = Vector3_f32( normalize( view_position - view_point ) );
	Vector3_f32 side = Vector3_f32( normalize( cross( forward, world_up ) ) );
	Vector3_f32 up = Vector3_f32( cross( side, forward ) );

	Matrix4x4_f32 result;
	result[0][0] = side.x;
	result[1][0] = side.y;
	result[2][0] = side.z;
	result[0][1] = up.x;
	result[1][1] = up.y;
	result[2][1] = up.z;
	result[0][2] = - forward.x;
	result[1][2] = - forward.y;
	result[2][2] = - forward.z;
	result[3][0] = - dot( side, view_position );
	result[3][1] = - dot( up, view_position );
	result[3][2] = dot( forward, view_position );

	return result;
}
