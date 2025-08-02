#include "camera.h"

#define QL_LOG_CHANNEL "Camera"
#include "log.h"

constexpr u64 CAMERAS_INITIAL_CAPACITY = 4;

struct G_Camera {
	Array< Camera > cameras;
} g_cameras;

bool cameras_init() {
	g_cameras.cameras = array_new< Camera >( sys_allocator, CAMERAS_INITIAL_CAPACITY );
	return true;
}

void cameras_shutdown() {
	array_free( &g_cameras.cameras );
}

u32 cameras_update() {
	u32 updated = 0;
	if ( !g_cameras.cameras.data )
		return updated;

	for ( u32 camera_idx = 0; camera_idx <= g_cameras.cameras.size; camera_idx += 1 ) {
		Camera *camera = &g_cameras.cameras.data[ camera_idx ];
		updated += camera_update( camera );
	}

	return updated;
}

Camera *
camera_create(
	StringView_ASCII name,
	Camera_Projection projection,
	Vector3_f32 position,
	Quaternion rotation,
	f32 fov,
	f32 z_near,
	f32 z_far,
	Vector2_f32 viewport
) {
	Camera camera = {
		.name = name,
		.bits = CameraBit_NeedsViewUpdate | CameraBit_NeedsProjectionUpdate,
		.position = position,
		.rotation = rotation,
		.fov = fov,
		.z_near = z_near,
		.z_far = z_far,
		.viewport = viewport
		// .view_matrix
		// .projection_matrix
	};

	bool is_orthographic = ( projection == CameraProjection_Orthographic );
	if ( is_orthographic )
		camera.bits |= CameraBit_IsOrthographic;

	u32 camera_idx = array_add( &g_cameras.cameras, camera );
	StringView_ASCII projection_name = ( is_orthographic ) ? "Orthographic" : "Perspective";
	log_info(
		"Created '" StringViewFormat "' (#%u, '" StringViewFormat "', %.fx%.f, fov: %.f, z_near: %f, z_far: %f).",
		StringViewArgument( camera.name ),
		camera_idx,
		StringViewArgument( projection_name ),
		camera.viewport.width,
		camera.viewport.height,
		camera.fov,
		camera.z_near,
		camera.z_far
	);
	return &g_cameras.cameras.data[ camera_idx ];
}

bool camera_destroy( Camera *camera ) {
	return false;
}

bool camera_update( Camera *camera ) {
	bool updated = false;
	if ( camera->bits & CameraBit_NeedsViewUpdate ) {
		camera_update_view( camera );
		camera->bits &= ~CameraBit_NeedsViewUpdate;
		updated = true;
		Assert( ( camera->bits & CameraBit_NeedsViewUpdate ) == 0 );
	}

	if ( camera->bits & CameraBit_NeedsProjectionUpdate ) {
		camera_update_projection( camera );
		camera->bits &= ~CameraBit_NeedsProjectionUpdate;
		updated = true;
		Assert( ( camera->bits & CameraBit_NeedsProjectionUpdate ) == 0 );
	}

	return updated;
}

void camera_update_view( Camera *camera ) {
	camera->view_matrix = camera_view_space_matrix2( camera->position, camera->rotation );
}

void camera_update_projection( Camera *camera ) {
	if ( camera->bits & CameraBit_IsOrthographic ) {
		camera->projection_matrix = camera_projection_orthographic(
			/*  x_min */ 0.0f,
			/*  x_max */ camera->viewport.width,
			/*  y_min */ 0.0f,
			/*  y_max */ camera->viewport.height,
			/* z_near */ camera->z_near,
			/*  z_far */ camera->z_far
		);
	} else {
		f32 aspect_ratio = camera->viewport.width / camera->viewport.height;
		camera->projection_matrix = camera_projection_perspective(
			/*          fov */ radians( camera->fov ),
			/* aspect_ratio */ aspect_ratio,
			/*       z_near */ camera->z_near,
			/*        z_far */ camera->z_far
		);
	}
}

Vector3_f32 camera_direction_right( Camera *camera ) {
	// return camera->rotation * WORLD_DIRECTION_RIGHT;
	return rotate_vector3_by_quaternion( camera->rotation, WORLD_DIRECTION_RIGHT );
}

Vector3_f32 camera_direction_up( Camera *camera ) {
	// return camera->rotation * WORLD_DIRECTION_UP;
	return rotate_vector3_by_quaternion( camera->rotation, WORLD_DIRECTION_UP );
}

Vector3_f32 camera_direction_forward( Camera *camera ) {
	// return camera->rotation * WORLD_DIRECTION_FORWARD;
	return rotate_vector3_by_quaternion( camera->rotation, WORLD_DIRECTION_FORWARD );
}

void camera_set_rotation_quaternion( Camera *camera, Quaternion quaternion ) {
	camera->rotation = quaternion;
	camera->bits |= CameraBit_NeedsViewUpdate;
}

void camera_rotate_by_quaternion( Camera *camera, Quaternion quaternion ) {
	// Apply in Local Space (otherwise it would be in Global Space).
	camera->rotation = quaternion_multiply( camera->rotation, quaternion );
	// camera_reconstruct_rotation( camera );
	camera->bits |= CameraBit_NeedsViewUpdate;
}

void camera_set_position( Camera *camera, Vector3_f32 position ) {
	camera->position = position;
	camera->bits |= CameraBit_NeedsViewUpdate;
}

void camera_move( Camera *camera, Vector3_f32 offset ) {
	camera->position += offset;
	camera->bits |= CameraBit_NeedsViewUpdate;
}

void camera_set_fov( Camera *camera, f32 fov ) {
	camera->fov = fov;
	camera->bits |= CameraBit_NeedsProjectionUpdate;
}

void camera_zoom( Camera *camera, f32 offset ) {
	camera->fov += offset;
	camera->bits |= CameraBit_NeedsProjectionUpdate;
}

void camera_set_clip_distance( Camera *camera, f32 z_near, f32 z_far ) {
	camera->z_near = z_near;
	camera->z_far = z_far;
	camera->bits |= CameraBit_NeedsProjectionUpdate;
}

void camera_look_at( Camera *camera, Vector3_f32 position ) {
	camera_update_view_and_rotation( camera, position, WORLD_DIRECTION_UP );
}

Matrix4x4_f32 camera_projection_perspective( f32 fov_vertical_radians, f32 aspect_ratio, f32 z_near, f32 z_far ) {
	// Right-handed, [-1, 1] OpenGL's depth range for the NDC (normalized device coordinates)
	f32 tan_half_fov = tan( fov_vertical_radians / 2.0f );

	Matrix4x4_f32 result = Matrix4x4_f32( 0.0f );
	result[ 0 ][ 0 ] = 1.0f / ( aspect_ratio * tan_half_fov );
	result[ 1 ][ 1 ] = 1.0f / tan_half_fov;
	result[ 2 ][ 2 ] = - ( z_far + z_near ) / ( z_far - z_near );
	result[ 2 ][ 3 ] = - 1.0f;
	result[ 3 ][ 2 ] = - ( 2.0f * z_far * z_near ) / ( z_far - z_near );

	return result;
}

Matrix4x4_f32 camera_projection_orthographic( f32 x_min, f32 x_max, f32 y_min, f32 y_max, f32 z_near, f32 z_far ) {
	// Right-handed, [-1, 1] OpenGL's depth range for the NDC (normalized device coordinates)
	// x_min = left
	// x_max = right
	// y_min = bottom
	// y_max = top

	Matrix4x4_f32 result = Matrix4x4_f32( 1.0f );
	result[ 0 ][ 0 ] = 2.0f / ( x_max - x_min );
	result[ 1 ][ 1 ] = 2.0f / ( y_max - y_min );
	result[ 2 ][ 2 ] = - 2.0f / ( z_far - z_near );
	result[ 3 ][ 0 ] = - ( x_max + x_min ) / ( x_max - x_min );
	result[ 3 ][ 1 ] = - ( y_max + y_min ) / ( y_max - y_min );
	result[ 3 ][ 2 ] = - ( z_far + z_near ) / ( z_far - z_near );

	return result;
}

Matrix4x4_f32 camera_view_space_matrix( Vector3_f32 view_position, Vector3_f32 view_target, Vector3_f32 world_up ) {
	// Right-handed
	Vector3_f32 forward = Vector3_f32( normalize( view_position - view_target ) );
	Vector3_f32 right = Vector3_f32( normalize( cross( forward, world_up ) ) );
	Vector3_f32 up = Vector3_f32( cross( right, forward ) );

	Matrix4x4_f32 result;
	result[0][0] = right.x;
	result[1][0] = right.y;
	result[2][0] = right.z;
	result[0][1] = up.x;
	result[1][1] = up.y;
	result[2][1] = up.z;
	result[0][2] = - forward.x;
	result[1][2] = - forward.y;
	result[2][2] = - forward.z;
	result[3][0] = - dot( right, view_position );
	result[3][1] = - dot( up, view_position );
	result[3][2] = dot( forward, view_position );

	return result;
}

Matrix4x4_f32 camera_view_space_matrix2( Vector3_f32 view_position, Quaternion rotation ) {
	// Right-handed
	Vector3_f32 forward = rotate_vector3_by_quaternion( rotation, WORLD_DIRECTION_FORWARD );
	Vector3_f32 right = rotate_vector3_by_quaternion( rotation, WORLD_DIRECTION_RIGHT );
	Vector3_f32 up = rotate_vector3_by_quaternion( rotation, WORLD_DIRECTION_UP );

	Matrix4x4_f32 result;
	result[0][0] = right.x;
	result[1][0] = right.y;
	result[2][0] = right.z;
	result[0][1] = up.x;
	result[1][1] = up.y;
	result[2][1] = up.z;
	result[0][2] = - forward.x;
	result[1][2] = - forward.y;
	result[2][2] = - forward.z;
	result[3][0] = - dot( right, view_position );
	result[3][1] = - dot( up, view_position );
	result[3][2] = dot( forward, view_position );

	return result;
}

Quaternion quaternion_from_basis_vectors_matrix( Matrix3x3_f32 m ) {
	// m[ 0 ] = vector right
	// m[ 1 ] = vector up
	// m[ 2 ] = vector forward

	// Trace helps decide the most numerically stable way
	// to compute the quaternion components. If:
	//   a. trace is large/positive - `w` scalar component dominates,
	//     compute it first.
	//   b. trace is small/negative - `x` or `y` or `z` imaginary component dominates,
	//     compute it first.
	// This method avoids division by small numbers, which amplifies precision loss.
	// The trace is related to the cosine of the rotation angle
	//   (for a rotation matrix, `trace = 1 + 2 * cos(theta)`).
	// A large trace (`trace > 0`) means the rotation angle is small,
	//   so `w` (the real part) dominates.
	// A small trace (`trace <= 0`) means the rotation is near 180 degrees
	//   around some axis, so one of the imaginary components (`x`, `y`, `z`) dominates.
	f32 trace = m[0][0] + m[1][1] + m[2][2];

	// Scaling factor is used to normalize the quaternion components.
	// It ensures the quaternion has a magnitude of 1 (unit quaternion),
	//   as in formula: `w^2  +  x^2  +  y^2  +  z^2  =  1`.
	// The value depends on the `trace` and the largest diagonal
	//   element of the rotation matrix.
	f32 scaling_factor;

	Quaternion q;
	if ( trace > 0.0f ) {
		// `w` dominates
		scaling_factor = sqrt( trace + 1.0f ) * 2.0f;
		q.x = ( m[2][1] - m[1][2] ) / scaling_factor;
		q.y = ( m[0][2] - m[2][0] ) / scaling_factor;
		q.z = ( m[1][0] - m[0][1] ) / scaling_factor;
		q.w = 0.25f * scaling_factor;
	} else if ( ( m[0][0] > m[1][1] ) && ( m[0][0] > m[2][2] ) ) {
		// `x` dominates
		scaling_factor = sqrt( 1.0f + m[0][0] - m[1][1] - m[2][2] ) * 2.0f;
		q.x = 0.25f * scaling_factor;
		q.y = ( m[0][1] + m[1][0] ) / scaling_factor;
		q.z = ( m[0][2] + m[2][0] ) / scaling_factor;
		q.w = ( m[2][1] - m[1][2] ) / scaling_factor;
	} else if ( m[1][1] > m[2][2] ) {
		// `y` dominates
		scaling_factor = sqrt( 1.0f + m[1][1] - m[0][0] - m[2][2] ) * 2.0f;
		q.x = ( m[0][1] + m[1][0] ) / scaling_factor;
		q.y = 0.25f * scaling_factor;
		q.z = ( m[1][2] + m[2][1] ) / scaling_factor;
		q.w = ( m[0][2] - m[2][0] ) / scaling_factor;
	} else {
		// `z` dominates
		scaling_factor = sqrt( 1.0f + m[2][2] - m[0][0] - m[1][1] ) * 2.0f;
		q.x = ( m[0][2] + m[2][1] ) / scaling_factor;
		q.y = ( m[1][2] + m[2][1] ) / scaling_factor;
		q.z = 0.25f * scaling_factor;
		q.w = ( m[1][0] - m[0][1] ) / scaling_factor;
	}

	return q;
}

void camera_update_view_and_rotation( Camera *camera, Vector3_f32 view_target, Vector3_f32 world_up ) {
	// Compute view matrix
	Vector3_f32 forward = rotate_vector3_by_quaternion( camera->rotation, WORLD_DIRECTION_FORWARD );
	Vector3_f32 right = rotate_vector3_by_quaternion( camera->rotation, WORLD_DIRECTION_RIGHT );
	Vector3_f32 up = rotate_vector3_by_quaternion( camera->rotation, WORLD_DIRECTION_UP );

	forward = normalize( forward );
	right = normalize( right );
	up = normalize( up );

	Matrix4x4_f32 &view = camera->view_matrix;
	view[0][0] = right.x;
	view[1][0] = right.y;
	view[2][0] = right.z;
	view[0][1] = up.x;
	view[1][1] = up.y;
	view[2][1] = up.z;
	view[0][2] = - forward.x;
	view[1][2] = - forward.y;
	view[2][2] = - forward.z;
	view[3][0] = - dot( right, camera->position );
	view[3][1] = - dot( up, camera->position );
	view[3][2] = dot( forward, camera->position );

	// Compute quaternion rotation
	camera->rotation = quaternion_from_basis_vectors( right, up, forward );
	camera->rotation = normalize( camera->rotation );
}

Vector3_f32 rotate_vector3_by_quaternion( Quaternion q, Vector3_f32 v ) {
	Vector3_f32 qv = Vector3_f32( q.x, q.y, q.z );
	Vector3_f32 uv = cross( qv, v );
	Vector3_f32 uuv = cross( qv, uv );
	Vector3_f32 result = v + (  ( uv * q.w )  + uuv  ) * 2.0f;
	return result;
}

Quaternion quaternion_multiply( Quaternion q1, Quaternion q2 ) {
	Quaternion &p = q1;
	Quaternion &q = q2;

	Quaternion result;
	result.w = p.w * q.w  -  p.x * q.x  -  p.y * q.y  -  p.z * q.z;
	result.x = p.w * q.x  +  p.x * q.w  +  p.y * q.z  -  p.z * q.y;
	result.y = p.w * q.y  +  p.y * q.w  +  p.z * q.x  -  p.x * q.z;
	result.z = p.w * q.z  +  p.z * q.w  +  p.x * q.y  -  p.y * q.x;
	return result;
}

Quaternion quaternion_normalize( Quaternion q ) {
    f32 length = sqrt( q.x * q.x  +  q.y * q.y  +  q.z * q.z  +  q.w * q.w );
    if ( length > 0.0f ) {
        f32 inverse_length = 1.0f / length;
        q.x *= inverse_length;
        q.y *= inverse_length;
        q.z *= inverse_length;
        q.w *= inverse_length;
    }
    return q;
}

Quaternion quaternion_from_axis_angle( Vector3_f32 axis, f32 angle ) {
	f32 half_angle = angle * 0.5f;
	f32 sin_half = sin( half_angle );
	f32 cos_half = cos( half_angle );

	Quaternion q;
	q.x = axis.x * sin_half;
	q.y = axis.y * sin_half;
	q.z = axis.z * sin_half;
	q.w = cos_half;
	q = normalize( q );
	return q;
}

void camera_reconstruct_rotation( Camera *camera ) {
	Vector3_f32 forward = camera_direction_forward( camera );
	Vector3_f32 up = camera_direction_up( camera );

	Vector3_f32 right = normalize( cross( up, forward ) );
	up = normalize( cross( forward, right ) );

	camera->rotation = quaternion_from_basis_vectors( right, up, forward );
}
