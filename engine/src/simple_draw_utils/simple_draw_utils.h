#ifndef __SIMPLE_DRAW_UTILS_H__
#define __SIMPLE_DRAW_UTILS_H__

#include "codeflow.h"
#include "frame_buffer_objects/frame_buffer_objects.h"

err_t init_simple_draw_utils();

err_t simple_draw_utils_draw_fbo_color_texture(fbo_t* fbo);
err_t simple_draw_utils_draw_fbo_depth_stencil_texture(fbo_t* fbo);

#endif // __SIMPLE_DRAW_UTILS_H__
