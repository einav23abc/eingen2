#include "game.h"

#include "utils/utils.h"
#include "shaders/shaders.h"
#include "simple_draw_utils/simple_draw_utils.h"

err_t render_game_world() {
    err_t err = NO_ERROR;
    quat_t quat_rotation = {0};
    shader_t* current_shader = NULL;

    glDisable(GL_CULL_FACE);
    
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

    simple_draw_utils_set_color(1, 0, 0, 1);
    RETHROW_IF_ERROR(simple_draw_utils_draw_cube(100, 0, 100, 10, 10, 10));

cleanup:
    return err;
}

static err_t game_render() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(use_camera(game_camera));
    RETHROW_IF_ERROR(use_shader(global_shader));
    // u_camera_position
    glUniform3f(global_shader->uniform_locations[3], game_camera->x, game_camera->y, game_camera->z);
    // u_sun_vector
    glUniform3f(global_shader->uniform_locations[4], 0.5, -2, 1);
    // u_sun_shadow_map_texture
    // bind_fbo_depth_stencil_texture(sun_shadow_map_fbo, global_shader->uniform_locations[6], 1);

    RETHROW_IF_ERROR(render_game_world());

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
