#include "game.h"

#include "utils/utils.h"
#include "simple_draw_utils/simple_draw_utils.h"

err_t render() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(use_fbo(outport_fbo));
    
    glClearColor(OUTPORT_BACKGROUND_COLOR_R, OUTPORT_BACKGROUND_COLOR_G, OUTPORT_BACKGROUND_COLOR_B, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    // actually render

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
