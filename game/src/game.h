#ifndef __GAME_H__
#define __GAME_H__

#include "engine.h"
#include "cameras/cameras.h"
#include "vec_mat_quat/vec_mat_quat.h"
#include "frame_buffer_objects/frame_buffer_objects.h"
#include "meshes_and_animations/meshes_and_animations.h"
#include "shaders/shaders.h"
#include "textures/textures.h"

#define OUTPORT_WIDTH (380)
#define OUTPORT_HEIGHT (260)

#define OUTPORT_BACKGROUND_COLOR_R (71.0/255)
#define OUTPORT_BACKGROUND_COLOR_G (65.0/255)
#define OUTPORT_BACKGROUND_COLOR_B (107.0/255)

extern fbo_t* outport_fbo;

extern camera_t* game_camera;
extern shader_t* global_shader;
extern texture_t* global_texture;

extern mesh_t* man_mesh;
extern animation_t* man_anim_t_pose;
extern animation_t* man_anim_run;

extern mesh_t* cube_mesh;

extern float sun_vector_x;
extern float sun_vector_y;
extern float sun_vector_z;
extern camera_t* sun_shadow_map_camera;
extern fbo_t* sun_shadow_map_fbo;
extern shader_t* sun_shadow_map_shader;

extern float player_vx;
extern float player_vz;
extern float player_x;
extern float player_y;
extern float player_z;
extern float player_ry;
extern animation_t* player_current_animation;
extern animation_t* player_last_animation;
extern float player_last_animation_frame;
extern float player_current_animation_frame;
extern float player_animation_transition_frame;

#endif // __GAME_H__
