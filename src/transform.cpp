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

Transform transform_identity() {
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

	transform_set_dirty( &transform, true );
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
	transform_set_dirty( transform, false );

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

void transform_set_dirty( Transform *transform, bool dirty ) {
	f32 value = ( dirty ) ? INFINITY : 0.0f;
	transform->model_matrix[ 0 ][ 3 ] = value;
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

Quaternion yxz_euler_to_quaternion_rotation( Vector3_f32 yxz_euler ) {
	f32 half_roll = yxz_euler.z / 2.0f;
	f32 half_pitch = yxz_euler.x / 2.0f;
	f32 half_yaw = yxz_euler.y / 2.0f;

	f32 cos_roll = cos( half_roll );
	f32 sin_roll = sin( half_roll );
	f32 cos_pitch = cos( half_pitch );
	f32 sin_pitch = sin( half_pitch );
	f32 cos_yaw = cos( half_yaw );
	f32 sin_yaw = sin( half_yaw );

	Quaternion result;
	result.w = cos_roll * cos_pitch * cos_yaw  +  sin_roll * cos_pitch * sin_yaw;
	result.x = cos_roll * sin_pitch * cos_yaw  +  sin_roll * cos_pitch * sin_yaw;
	result.y = cos_roll * cos_pitch * sin_yaw  +  sin_roll * sin_pitch * cos_yaw;
	result.z = sin_roll * cos_pitch * cos_yaw  +  cos_roll * sin_pitch * sin_yaw;
	return result;
}

Vector3_f32 quaternion_to_yxz_euler_rotation( Quaternion quaternion ) {
	f32 x = quaternion.x;
	f32 y = quaternion.y;
	f32 z = quaternion.z;
	f32 w = quaternion.w;

	f32 sin_pitch = 2.0f * ( w * x  -  y * z );
	f32 pitch;
	// Handle gimbal lock
	if      ( sin_pitch >=  1.0f )  pitch = PI / 2.0f;
	else if ( sin_pitch <= -1.0f )  pitch = -PI / 2.0f;
	else                            pitch = asin( sin_pitch );

	f32 roll = atan2(
		       2.0f * ( w * z  +  x * y ),
		1.0f - 2.0f * ( z * z  +  x * x )
	);

	f32 yaw = atan2(
		       2.0f * ( w * y  +  x * z ),
		1.0f - 2.0f * ( y * y  +  x * x )
	);

	Vector3_f32 yxz_euler;
	yxz_euler.x = pitch;
	yxz_euler.y = yaw;
	yxz_euler.z = roll;
	return yxz_euler;
}

Quaternion normalize( Quaternion q ) {
	f32 length = sqrt( dot( q, q ) );
	if ( length < 0 )
		return Quaternion( 0.0f, 0.0f, 0.0f, 1.0f );
	
	f32 one_over_length = 1.0f / length;
	Quaternion result;
	result.x = q.x * one_over_length;
	result.y = q.y * one_over_length;
	result.z = q.z * one_over_length;
	result.w = q.w * one_over_length;
	return result;
}
