#include "game.h"

#include "utils/utils.h"
#include "shaders/shaders.h"
#include "simple_draw_utils/simple_draw_utils.h"

err_t cube_draw(float x, float y, float z,
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

err_t render_game_world() {
    err_t err = NO_ERROR;
    shader_t* current_shader = NULL;

    glDisable(GL_CULL_FACE);
    
    RETHROW_IF_ERROR(get_current_shader(&current_shader));

    RETHROW_IF_ERROR(bind_texture_to_uniform(global_texture, current_shader->u_texture_loc, 0));

    RETHROW_IF_ERROR(cube_draw(0, 0, 0, 200, 10, 200, 0, 0, 0));

cleanup:
    return err;
}

static err_t game_render() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(use_fbo(outport_fbo));
    RETHROW_IF_ERROR(use_camera(game_camera));
    RETHROW_IF_ERROR(use_shader(global_shader));
    // u_camera_position
    glUniform3f(global_shader->uniform_locations[3], game_camera->x, game_camera->y, game_camera->z);

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
