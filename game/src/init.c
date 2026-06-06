#include "game.h"

#include "global_shader/global_shader.h"

fbo_t* outport_fbo = NULL;

camera_t* game_camera = NULL;
shader_t* global_shader = NULL;
texture_t* global_texture = NULL;

mesh_t* man_mesh = NULL;
animation_t* man_anim_t_pose = NULL;
animation_t* man_anim_run = NULL;

float player_vx = 0;
float player_vz = 0;
float player_x = 0;
float player_y = 0;
float player_z = 0;
float player_ry = 0;
animation_t* player_current_animation = NULL;
animation_t* player_last_animation = NULL;
float player_last_animation_frame = 0;
float player_current_animation_frame = 0;
float player_animation_transition_frame = 0;

err_t init() {
    err_t err = NO_ERROR;
    const quat_vec_vec_t man_anim_transform_qvv = {
        .rot = quat_from_axis_angles_yzx(M_PI*1.5, 0, 0),
        .scale = (vec3_t) {
            .x = 1,
            .y = 1,
            .z = 1,
        },
        .pos = (vec3_t) {
            .x = 0,
            .y = -4.0,
            .z = -0.3
        }
    };

    RETHROW_IF_ERROR(create_camera(
        &game_camera,
        0, 0, 0,
        0, 0, 0,
        OUTPORT_WIDTH, OUTPORT_HEIGHT, 1600,
        0.01, 32000,
        1, 60,
        0, 0, OUTPORT_WIDTH, OUTPORT_HEIGHT
    ));

    RETHROW_IF_ERROR(create_fbo(
        &outport_fbo,
        OUTPORT_WIDTH, OUTPORT_HEIGHT,
        TEXTURE_COLOR_FBO_ATTACHMENT,
        GL_RGB,
        DEPTH_STENCIL_TEXTURE_SECONDERY_FBO_ATTACHMENT
    ));

    RETHROW_IF_ERROR(mesh_from_collada_dae(&man_mesh, "./game/models/man_rigged_t_pose.dae", 0));
    
    RETHROW_IF_ERROR(animation_from_collada_dae_ext(
        &man_anim_t_pose,
        "./game/models/man_rigged_t_pose.dae",
        man_mesh->joints, man_mesh->joints_amount,
        man_anim_transform_qvv
    ));

    RETHROW_IF_ERROR(animation_from_collada_dae_ext(
        &man_anim_run,
        "./game/models/man_rigged_run.dae",
        man_mesh->joints, man_mesh->joints_amount,
        man_anim_transform_qvv
    ));

    RETHROW_IF_ERROR(create_shader(
        &global_shader,
        global_shader_vertex_shader,
        global_shader_fragment_shader,
        "in_vertex_position\0in_vertex_texcoord\0in_vertex_normal\0in_vertex_joint_id\0in_vertex_joint_wheight", 5,
        "u_position\0u_scale\0u_quat_rotation\0u_camera_position\0u_sun_vector\0u_sun_shadow_map_wvp_mat\0u_sun_shadow_map_texture", 7
    ));

    RETHROW_IF_ERROR(load_texture(&global_texture, "./game/textures/global_texture.png"));

    player_last_animation = man_anim_t_pose;
    player_current_animation = man_anim_t_pose;

cleanup:
    return err;
}
