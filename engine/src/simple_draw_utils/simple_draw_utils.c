#ifndef __SIMPLE_DRAW_UTILS_H__
#define __SIMPLE_DRAW_UTILS_H__

#include "simple_draw_utils.h"

#include <stdbool.h>

#include "shaders/shaders.h"
#include "frame_buffer_objects/frame_buffer_objects.h"
#include "meshes_and_animations/meshes_and_animations.h"

#include "simple_draw_utils/screen_quad_mesh_shader/screen_quad_mesh_shader.h"

static shader_t* simple_draw_utils_screen_quad_mesh_shader = NULL;
static mesh_t* simple_draw_utils_screen_quad_mesh = NULL;


static err_t generate_simple_draw_utils_screen_quad_mesh() {
    err_t err = NO_ERROR;

    float vertices_data_arr[] = {
        0,0,
        0,1,
        1,1,
        1,0
    };

    vbo_data_t vbo_datas_arr[1] = {
        {
            .data_array_size = sizeof(vertices_data_arr),
            .data_array = (void*)vertices_data_arr,
            .size = 2,
            .type = GL_FLOAT,
            .stride = 2*sizeof(float),
            .divisor = 0
        }
    };

    uint32_t indices_array[] = {
        1, 0, 2,
        2, 0, 3
    };

    RETHROW_IF_ERROR(generate_mesh(
        &simple_draw_utils_screen_quad_mesh,
        vbo_datas_arr, 1,
        indices_array, 6,
        0, 0
    ));
    // save_mesh_to_c_file(simple_draw_utils_screen_quad_mesh, "simple_draw_utils_screen_quad_mesh", "./engine/simple_draw_utils/simple_draw_utils_screen_quad_mesh.c");

cleanup:
    return err;
}

err_t init_simple_draw_utils() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(generate_simple_draw_utils_screen_quad_mesh());

    RETHROW_IF_ERROR(create_shader(
        &simple_draw_utils_screen_quad_mesh_shader,
        screen_quad_mesh_shader_vertex_shader,
        screen_quad_mesh_shader_fragment_shader,
        "in_vertex_data", 1,
        "", 0
    ));

cleanup:
    return err;
}

static err_t simple_draw_utils_draw_fbo_texture_internal(fbo_t* fbo, bool is_depth_stencil_texture) {
    err_t err = NO_ERROR;
    uint32_t depth_test_was_enabled = glIsEnabled(GL_DEPTH_TEST);

    CHECK_FBO(fbo);

    glDisable(GL_DEPTH_TEST);
    
    RETHROW_IF_ERROR(use_shader(simple_draw_utils_screen_quad_mesh_shader));
    if (is_depth_stencil_texture) {
        RETHROW_IF_ERROR(bind_fbo_depth_stencil_texture(fbo, simple_draw_utils_screen_quad_mesh_shader->u_texture_loc, 0));
    } else {
        RETHROW_IF_ERROR(bind_fbo_color_texture(fbo, simple_draw_utils_screen_quad_mesh_shader->u_texture_loc, 0));
    }
    RETHROW_IF_ERROR(draw_mesh(simple_draw_utils_screen_quad_mesh));

cleanup:
    if (depth_test_was_enabled) {
        glEnable(GL_DEPTH_TEST);
    }

    return err;
}

err_t simple_draw_utils_draw_fbo_color_texture(fbo_t* fbo) {
    return simple_draw_utils_draw_fbo_texture_internal(fbo, false);
}

err_t simple_draw_utils_draw_fbo_depth_stencil_texture(fbo_t* fbo) {
    return simple_draw_utils_draw_fbo_texture_internal(fbo, true);
}

#endif // __SIMPLE_DRAW_UTILS_H__
