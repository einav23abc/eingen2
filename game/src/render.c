#include "game.h"

#include <stdbool.h>

#include "utils/utils.h"
#include "shaders/shaders.h"
#include "simple_draw_utils/simple_draw_utils.h"

static err_t draw_cube(float x, float y, float z,
                       float w, float h, float d,
                       float rx, float ry, float rz) {
    err_t err = NO_ERROR;
    shader_t* current_shader = NULL;

    RETHROW_IF_ERROR(get_current_shader(&current_shader));

    // u_position
    glUniform3f(current_shader->uniform_locations[0], x, y, z);
    // u_scale
    glUniform3f(current_shader->uniform_locations[1], w, h, d);
    // u_quat_rotation
    quat_t quat_rotation = quat_from_axis_angles_yzx(-rx, -ry, -rz);
    glUniform4f(current_shader->uniform_locations[2], quat_rotation.x, quat_rotation.y, quat_rotation.z, quat_rotation.w);
    
    RETHROW_IF_ERROR(draw_mesh(cube_mesh));

cleanup:
    return err;
}

static err_t draw_plain(float x, float y, float z,
                        float w, float d,
                        float rx, float ry, float rz) {
    err_t err = NO_ERROR;
    shader_t* current_shader = NULL;

    RETHROW_IF_ERROR(get_current_shader(&current_shader));

    glDisable(GL_CULL_FACE);

    // u_position
    glUniform3f(current_shader->uniform_locations[0], x, y, z);
    // u_scale
    glUniform3f(current_shader->uniform_locations[1], w, 1, d);
    // u_quat_rotation
    quat_t quat_rotation = quat_from_axis_angles_yzx(-rx, -ry, -rz);
    glUniform4f(current_shader->uniform_locations[2], quat_rotation.x, quat_rotation.y, quat_rotation.z, quat_rotation.w);
    
    RETHROW_IF_ERROR(draw_mesh(plain_mesh));

cleanup:
    glEnable(GL_CULL_FACE);

    return err;
}

static err_t set_global_uniforms() {
    err_t err = NO_ERROR;
    shader_t* current_shader = NULL;

    RETHROW_IF_ERROR(get_current_shader(&current_shader));

    // u_camera_position
    glUniform3f(current_shader->uniform_locations[3], game_camera->x, game_camera->y, game_camera->z);
    // u_sun_vector
    glUniform3f(current_shader->uniform_locations[4], sun_vector_x, sun_vector_y, sun_vector_z);
    // u_sun_shadow_map_wvp_mat
    glUniformMatrix4fv(current_shader->uniform_locations[5], 1, 0, sun_shadow_map_camera->wvp_mat.mat);
    // u_sun_shadow_map_texture
    RETHROW_IF_ERROR(bind_fbo_depth_stencil_texture(sun_shadow_map_fbo, current_shader->uniform_locations[6], 1));

cleanup:
    return err;
}

static err_t draw_player() {
    err_t err = NO_ERROR;
    quat_t quat_rotation = {0};
    shader_t* current_shader = NULL;

    RETHROW_IF_ERROR(get_current_shader(&current_shader));

    RETHROW_IF_ERROR(bind_texture_to_uniform(global_texture, current_shader->u_texture_loc, 0));
    // u_position
    glUniform3f(current_shader->uniform_locations[0], player_x, player_y, player_z);
    // u_scale
    glUniform3f(current_shader->uniform_locations[1], 5, 5, 5);
    // u_quat_rotation
    quat_rotation = quat_from_axis_angles_yzx(0, -player_ry, 0);
    glUniform4f(current_shader->uniform_locations[2], quat_rotation.x, quat_rotation.y, quat_rotation.z, quat_rotation.w);

    RETHROW_IF_ERROR(pose_mesh_set_from_animation(man_mesh, player_last_animation, fmod(player_last_animation_frame * 0.015, 0.75)));
    RETHROW_IF_ERROR(pose_mesh_mix_from_animation(man_mesh, player_current_animation, fmod(player_current_animation_frame * 0.015, 0.75), (player_animation_transition_frame * 0.015)/0.25));
    
    // RETHROW_IF_ERROR(pose_mesh_set_from_animation(man_mesh, man_anim_run, 0));
    RETHROW_IF_ERROR(draw_mesh_posed(man_mesh));
    // RETHROW_IF_ERROR(draw_mesh(man_mesh));

cleanup:
    return err;
}

static err_t render_game_world(bool should_set_shaders) {
    err_t err = NO_ERROR;

    glDisable(GL_CULL_FACE);
    
    if (should_set_shaders) {
        RETHROW_IF_ERROR(use_shader(global_shader));
        RETHROW_IF_ERROR(set_global_uniforms());
    }
    RETHROW_IF_ERROR(draw_player());
    RETHROW_IF_ERROR(draw_cube(20, 45, 20, 20, 100, 20, M_PI * 0.075, 0, M_PI * 0.025));
    
    if (should_set_shaders) {
        RETHROW_IF_ERROR(use_shader(grass_shader));
        RETHROW_IF_ERROR(set_global_uniforms());
    }
    RETHROW_IF_ERROR(draw_plain(0, 0, 0, 2000, 2000, 0, 0, 0));

cleanup:
    return err;
}

static err_t game_render() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(use_fbo(sun_shadow_map_fbo));
    glClear(GL_DEPTH_BUFFER_BIT);
    RETHROW_IF_ERROR(use_camera(sun_shadow_map_camera));
    // RETHROW_IF_ERROR(use_shader(sun_shadow_map_shader));
    RETHROW_IF_ERROR(render_game_world(true));


    RETHROW_IF_ERROR(use_fbo(outport_fbo));
    RETHROW_IF_ERROR(use_camera(game_camera));

    RETHROW_IF_ERROR(render_game_world(true));

cleanup:
    return err;
}

err_t render() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(use_fbo(outport_fbo));
    
    glClearColor(OUTPORT_BACKGROUND_COLOR_R, OUTPORT_BACKGROUND_COLOR_G, OUTPORT_BACKGROUND_COLOR_B, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    RETHROW_IF_ERROR(game_render());

    RETHROW_IF_ERROR(use_default_fbo());

    uint32_t pixel_scale = uintmin(get_window_drawable_width()/OUTPORT_WIDTH, get_window_drawable_height()/OUTPORT_HEIGHT);
    uint32_t w = OUTPORT_WIDTH*pixel_scale;
    uint32_t h = OUTPORT_HEIGHT*pixel_scale;
    if (pixel_scale < 1) {
        float fpixel_scale = fmin(((float)get_window_drawable_width())/OUTPORT_WIDTH, ((float)get_window_drawable_height())/OUTPORT_HEIGHT);
        w = OUTPORT_WIDTH * fpixel_scale;
        h = OUTPORT_HEIGHT * fpixel_scale;
    }
    glViewport((get_window_drawable_width() - w) * 0.5, (get_window_drawable_height() - h) * 0.5, w, h);

    RETHROW_IF_ERROR(simple_draw_utils_draw_fbo_color_texture(outport_fbo));

cleanup:
    return err;
}
