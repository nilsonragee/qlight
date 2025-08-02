#ifndef QLIGHT_CAMERA_H
#define QLIGHT_CAMERA_H

#include "common.h"
#include "string.h"
#include "math.h"
#include "transform.h"

enum Camera_Bit : u64 {
	CameraBit_IsOrthographic = ( 1 << 0 ),
	CameraBit_NeedsViewUpdate = ( 1 << 1 ),
	CameraBit_NeedsProjectionUpdate = ( 1 << 2 )
};
typedef u64 Camera_Bits;

enum Camera_Projection : u8 {
	CameraProjection_Perspective = 0,
	CameraProjection_Orthographic = 1,

	CameraProjection_COUNT,
	CameraProjection_INVALID = U8_MAX
};

struct Camera {
	StringView_ASCII name;
	Camera_Bits bits;

	// --- View parameters
	Vector3_f32 position;
	Quaternion rotation;

	// --- Projection parameters
	f32 orthographic_size; // In world units: [-size; size]. Corrected with viewport's aspect ratio.
	f32 fov;
	f32 z_near; // near_clip_plane_distance
	f32 z_far; // far_clip_plane_distance
	Vector2_f32 viewport;

	// --- These are recalculated, don't change manually!
	Matrix4x4_f32 view_matrix;
	Matrix4x4_f32 projection_matrix;
};

bool cameras_init();
void cameras_shutdown();
u32 cameras_update();

Camera *
camera_create(
	StringView_ASCII name,
	Camera_Projection projection,
	Vector3_f32 position,
	Quaternion rotation,
	f32 orthographic_size,
	f32 fov,
	f32 z_near,
	f32 z_far,
	Vector2_f32 viewport
);
bool camera_destroy( Camera *camera );
Camera * camera_find( StringView_ASCII name );

bool camera_update( Camera *camera ); // Updates view and projection conditionally
void camera_update_view( Camera *camera ); // Always updates view
void camera_update_projection( Camera *camera ); // Always updates projection
void camera_update_view_and_rotation( Camera *camera, Vector3_f32 view_target, Vector3_f32 world_up );

Vector3_f32 camera_direction_right( Camera *camera );
Vector3_f32 camera_direction_up( Camera *camera );
Vector3_f32 camera_direction_forward( Camera *camera );

void camera_set_rotation_quaternion( Camera *camera, Quaternion quaternion );
inline void camera_set_rotation_yxz_euler( Camera *camera, Vector3_f32 yxz_euler ) {
	Quaternion rotation = yxz_euler_to_quaternion_rotation( yxz_euler );
	camera_set_rotation_quaternion( camera, rotation );
}
inline void camera_set_rotation_yxz_euler_degrees( Camera *camera, Vector3_f32 yxz_euler ) {
	Quaternion rotation = yxz_euler_degrees_to_quaternion_rotation( yxz_euler );
	camera_set_rotation_quaternion( camera, rotation );
}

void camera_rotate_by_quaternion( Camera *camera, Quaternion quaternion );
inline void camera_rotate_by_yxz_euler( Camera *camera, Vector3_f32 yxz_euler ) {
	Quaternion rotation = yxz_euler_to_quaternion_rotation( yxz_euler );
	camera_rotate_by_quaternion( camera, rotation );
}
inline void camera_rotate_by_yxz_euler_degrees( Camera *camera, Vector3_f32 yxz_euler ) {
	Quaternion rotation = yxz_euler_degrees_to_quaternion_rotation( yxz_euler );
	camera_rotate_by_quaternion( camera, rotation );
};

void camera_set_position( Camera *camera, Vector3_f32 position );
void camera_move( Camera *camera, Vector3_f32 offset );

void camera_set_fov( Camera *camera, f32 fov );
void camera_zoom( Camera *camera, f32 offset );

void camera_set_clip_distance( Camera *camera, f32 z_near, f32 z_far );

void camera_look_at( Camera *camera, Vector3_f32 position );

// Right-handed, [-1, 1] OpenGL's depth range for the NDC (normalized device coordinates)
Matrix4x4_f32 camera_projection_perspective( f32 fov_vertical_radians, f32 aspect_ratio, f32 z_near, f32 z_far );

// Right-handed, [-1, 1] OpenGL's depth range for the NDC (normalized device coordinates)
Matrix4x4_f32 camera_projection_orthographic( f32 x_min, f32 x_max, f32 y_min, f32 y_max, f32 z_near, f32 z_far );

// glm::lookAt( Vector3 eye, Vector3 center, Vector3 up );
Matrix4x4_f32 camera_view_space_matrix( Vector3_f32 view_position, Vector3_f32 view_target, Vector3_f32 world_up );
Matrix4x4_f32 camera_view_space_matrix2( Vector3_f32 view_position, Quaternion rotation );

Quaternion quaternion_from_basis_vectors_matrix( Matrix3x3_f32 m );
inline Quaternion quaternion_from_basis_vectors( Vector3_f32 right, Vector3_f32 up, Vector3_f32 forward ) {
	Matrix3x3_f32 m;
	m[ 0 ] = right;
	m[ 1 ] = up;
	m[ 2 ] = forward;
	return quaternion_from_basis_vectors_matrix( m );
}

Vector3_f32 rotate_vector3_by_quaternion( Quaternion quaternion, Vector3_f32 direction );

Quaternion quaternion_multiply( Quaternion q1, Quaternion q2 );
Quaternion quaternion_normalize( Quaternion q );
Quaternion quaternion_from_axis_angle( Vector3_f32 axis, f32 angle );

void camera_reconstruct_rotation( Camera *camera );

#endif /* QLIGHT_CAMERA_H */
