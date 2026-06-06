#ifndef __SIMPLE_DRAW_UTILS_H__
#define __SIMPLE_DRAW_UTILS_H__

#include "codeflow.h"
#include "frame_buffer_objects/frame_buffer_objects.h"

err_t init_simple_draw_utils();

void simple_draw_utils_set_color(float r, float g, float b, float a);

err_t simple_draw_utils_draw_cube(float x, float y, float z, float w, float h, float d);

err_t simple_draw_utils_draw_fbo_color_texture(fbo_t* fbo);
err_t simple_draw_utils_draw_fbo_depth_stencil_texture(fbo_t* fbo);

#endif // __SIMPLE_DRAW_UTILS_H__
