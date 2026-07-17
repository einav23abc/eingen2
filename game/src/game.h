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

extern mesh_t* cube_mesh;

#endif // __GAME_H__
