#include "game.h"

#include "global_shader/global_shader.h"

fbo_t* outport_fbo = NULL;

camera_t* game_camera = NULL;
shader_t* global_shader = NULL;
texture_t* global_texture = NULL;

mesh_t* cube_mesh = NULL;

static err_t initialize_cube_mesh() {
    err_t err = NO_ERROR;

    float vertices_position_arr[] = {
        // front
        -0.5,-0.5,-0.5,
        -0.5, 0.5,-0.5,
        0.5, 0.5,-0.5,
        0.5,-0.5,-0.5,
        // back
        -0.5,-0.5, 0.5,
        -0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5,-0.5, 0.5,
        // bottom
        -0.5,-0.5,-0.5,
        -0.5,-0.5, 0.5,
        0.5,-0.5, 0.5,
        0.5,-0.5,-0.5,
        // top
        -0.5, 0.5,-0.5,
        -0.5, 0.5, 0.5,
        0.5, 0.5, 0.5,
        0.5, 0.5,-0.5,
        // left
        -0.5,-0.5,-0.5,
        -0.5, 0.5,-0.5,
        -0.5, 0.5, 0.5,
        -0.5,-0.5, 0.5,
        // right
        0.5,-0.5,-0.5,
        0.5, 0.5,-0.5,
        0.5, 0.5, 0.5,
        0.5,-0.5, 0.5,
    };
    float vertices_texcoord_arr[] = {
        // front
        0,1,
        0,0,
        1,0,
        1,1,
        // back
        1,1,
        1,0,
        0,0,
        0,1,
        // bottom
        0,1,
        0,0,
        1,0,
        1,1,
        // top
        0,1,
        0,0,
        1,0,
        1,1,
        // left
        1,1,
        1,0,
        0,0,
        0,1,
        // right
        0,1,
        0,0,
        1,0,
        1,1,
    };
    float vertices_normal_arr[] = {
        // front
        0,0,-1,
        0,0,-1,
        0,0,-1,
        0,0,-1,
        // back
        0,0,1,
        0,0,1,
        0,0,1,
        0,0,1,
        // bottom
        0,-1,0,
        0,-1,0,
        0,-1,0,
        0,-1,0,
        // top
        0,1,0,
        0,1,0,
        0,1,0,
        0,1,0,
        // left
        -1,0,0,
        -1,0,0,
        -1,0,0,
        -1,0,0,
        // right
        1,0,0,
        1,0,0,
        1,0,0,
        1,0,0,
    };
    vbo_data_t vbo_datas_arr[3] = {
        {
            .data_array_size = sizeof(vertices_position_arr),
            .data_array = (void*)vertices_position_arr,
            .size = 3,
            .type = GL_FLOAT,
            .stride = 3*sizeof(float),
            .divisor = 0
        },
        {
            .data_array_size = sizeof(vertices_texcoord_arr),
            .data_array = (void*)vertices_texcoord_arr,
            .size = 2,
            .type = GL_FLOAT,
            .stride = 2*sizeof(float),
            .divisor = 0
        },
        {
            .data_array_size = sizeof(vertices_normal_arr),
            .data_array = (void*)vertices_normal_arr,
            .size = 3,
            .type = GL_FLOAT,
            .stride = 3*sizeof(float),
            .divisor = 0
        }
    };

    uint32_t indices_array[] = {
        // front
        1, 0, 2,
        2, 0, 3,
        // back
        4, 5, 6,
        4, 6, 7,
        // bottom
        8, 9,10,
        8,10,11,
        // top
        13,12,14,
        14,12,15,
        // left
        16,17,18,
        16,18,19,
        // right
        21,20,22,
        22,20,23,
    };
    
    RETHROW_IF_ERROR(generate_mesh(&cube_mesh, vbo_datas_arr, 3, indices_array, 36, 0, 0));

cleanup:
    return err;
}

err_t init() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(create_camera(
        &game_camera,
        0, 0, 0,
        0, 0, 0,
        OUTPORT_WIDTH, OUTPORT_HEIGHT, 1600,
        -32000, 32000,
        0, 60,
        0, 0, OUTPORT_WIDTH, OUTPORT_HEIGHT
    ));

    RETHROW_IF_ERROR(create_fbo(
        &outport_fbo,
        OUTPORT_WIDTH, OUTPORT_HEIGHT,
        TEXTURE_COLOR_FBO_ATTACHMENT,
        GL_RGB,
        DEPTH_STENCIL_TEXTURE_SECONDERY_FBO_ATTACHMENT
    ));

    RETHROW_IF_ERROR(initialize_cube_mesh());

    RETHROW_IF_ERROR(create_shader(
        &global_shader,
        global_shader_vertex_shader,
        global_shader_fragment_shader,
        "in_vertex_position\0in_vertex_texcoord\0in_vertex_normal\0in_vertex_joint_id\0in_vertex_joint_wheight", 5,
        "u_position\0u_scale\0u_quat_rotation\0u_camera_position", 4
    ));

    RETHROW_IF_ERROR(load_texture(&global_texture, "./game/textures/global_texture.png"));

cleanup:
    return err;
}
