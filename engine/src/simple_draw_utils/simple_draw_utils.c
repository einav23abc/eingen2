#include "simple_draw_utils.h"

#include <stdbool.h>

#include "shaders/shaders.h"
#include "frame_buffer_objects/frame_buffer_objects.h"
#include "meshes_and_animations/meshes_and_animations.h"

#include "simple_draw_utils/screen_quad_mesh_shader/screen_quad_mesh_shader.h"
#include "simple_draw_utils/colored_object_shader/colored_object_shader.h"

static float simple_draw_utils_color_r = 1.0;
static float simple_draw_utils_color_g = 1.0;
static float simple_draw_utils_color_b = 1.0;
static float simple_draw_utils_color_a = 1.0;
static shader_t* simple_draw_utils_colored_object_shader = NULL;
static shader_t* simple_draw_utils_screen_quad_mesh_shader = NULL;
static mesh_t* simple_draw_utils_cube_mesh = NULL;
static mesh_t* simple_draw_utils_screen_quad_mesh = NULL;


static err_t initialize_simple_draw_utils_screen_quad_mesh() {
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

static err_t initialize_simple_draw_utils_cube_mesh() {
    err_t err = NO_ERROR;

    float vertices_position_arr[] = {
        0,0,0,
        1,0,0,
        0,1,0,
        1,1,0,

        0,0,1,
        1,0,1,
        0,1,1,
        1,1,1
    };

    vbo_data_t vbo_datas_arr[1] = {
        {
            .data_array_size = sizeof(vertices_position_arr),
            .data_array = (void*)vertices_position_arr,
            .size = 3,
            .type = GL_FLOAT,
            .stride = 3*sizeof(float),
            .divisor = 0
        }
    };

    uint32_t indices_array[] = {
        //Top
        2, 6, 7,
        2, 3, 7,
        //Bottom
        0, 4, 5,
        0, 1, 5,
        //Left
        0, 2, 6,
        0, 4, 6,
        //Right
        1, 3, 7,
        1, 5, 7,
        //Front
        0, 2, 3,
        0, 1, 3,
        //Back
        4, 6, 7,
        4, 5, 7
    };

    RETHROW_IF_ERROR(generate_mesh(&simple_draw_utils_cube_mesh, vbo_datas_arr, 1, indices_array, 6*6, 0, 0));

cleanup:
    return err;
}

void simple_draw_utils_set_color(float r, float g, float b, float a) {
    simple_draw_utils_color_r = r;
    simple_draw_utils_color_g = g;
    simple_draw_utils_color_b = b;
    simple_draw_utils_color_a = a;
}

err_t init_simple_draw_utils() {
    err_t err = NO_ERROR;

    simple_draw_utils_set_color(1.0, 1.0, 1.0, 1.0);

    RETHROW_IF_ERROR(initialize_simple_draw_utils_screen_quad_mesh());
    RETHROW_IF_ERROR(initialize_simple_draw_utils_cube_mesh());

    RETHROW_IF_ERROR(create_shader(
        &simple_draw_utils_screen_quad_mesh_shader,
        screen_quad_mesh_shader_vertex_shader,
        screen_quad_mesh_shader_fragment_shader,
        "in_vertex_data", 1,
        "", 0
    ));

    RETHROW_IF_ERROR(create_shader(
        &simple_draw_utils_colored_object_shader,
        colored_object_shader_vertex_shader,
        colored_object_shader_fragment_shader,
        "in_vertex_position\0in_vertex_texcoord", 2,
        "u_position\0u_size\0u_color", 3
    ));

cleanup:
    return err;
}

err_t simple_draw_utils_draw_cube(float x, float y, float z, float w, float h, float d) {
    err_t err = NO_ERROR;
    shader_t* last_shader = NULL;

    RETHROW_IF_ERROR(get_current_shader(&last_shader));

    glDisable(GL_CULL_FACE);

    RETHROW_IF_ERROR(use_shader(simple_draw_utils_colored_object_shader));
    glUniform3f(simple_draw_utils_colored_object_shader->uniform_locations[0], x, y, z);
    glUniform3f(simple_draw_utils_colored_object_shader->uniform_locations[1], w, h, d);
    glUniform4f(simple_draw_utils_colored_object_shader->uniform_locations[2],
        simple_draw_utils_color_r,
        simple_draw_utils_color_g,
        simple_draw_utils_color_b,
        simple_draw_utils_color_a
    );
    RETHROW_IF_ERROR(draw_mesh(simple_draw_utils_cube_mesh));

    glEnable(GL_CULL_FACE);

cleanup:
    if (last_shader != NULL) {
        use_shader(last_shader);
    }

    return err;
}

static err_t simple_draw_utils_draw_fbo_texture_internal(fbo_t* fbo, bool is_depth_stencil_texture) {
    err_t err = NO_ERROR;
    shader_t* last_shader = NULL;
    uint32_t depth_test_was_enabled = glIsEnabled(GL_DEPTH_TEST);

    CHECK_FBO(fbo);

    RETHROW_IF_ERROR(get_current_shader(&last_shader));

    glDisable(GL_DEPTH_TEST);
    
    RETHROW_IF_ERROR(use_shader(simple_draw_utils_screen_quad_mesh_shader));
    if (is_depth_stencil_texture) {
        RETHROW_IF_ERROR(bind_fbo_depth_stencil_texture(fbo, simple_draw_utils_screen_quad_mesh_shader->u_texture_loc, 0));
    } else {
        RETHROW_IF_ERROR(bind_fbo_color_texture(fbo, simple_draw_utils_screen_quad_mesh_shader->u_texture_loc, 0));
    }
    RETHROW_IF_ERROR(draw_mesh(simple_draw_utils_screen_quad_mesh));

cleanup:
    if (last_shader != NULL) {
        use_shader(last_shader);
    }
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
