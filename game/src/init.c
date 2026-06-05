#include "game.h"

fbo_t* outport_fbo = NULL;

err_t init() {
    err_t err = NO_ERROR;

    RETHROW_IF_ERROR(create_fbo(
        &outport_fbo,
        OUTPORT_WIDTH, OUTPORT_HEIGHT,
        TEXTURE_COLOR_FBO_ATTACHMENT,
        GL_RGB,
        NO_SECONDERY_FBO_ATTACHMENT
    ));

cleanup:
    return err;
}
