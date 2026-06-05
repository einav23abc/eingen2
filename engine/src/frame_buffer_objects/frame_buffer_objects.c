#include "frame_buffer_objects.h"

#include "macros.h"
#include "codeflow.h"
#include "gl_macros.h"
#include "../cameras/cameras.h"

// https://learnopengl.com/Advanced-OpenGL/Framebuffers


#ifndef _MAX_FBO_AMOUNT_
#define _MAX_FBO_AMOUNT_ (128)
#endif

#define IS_FBO_INDEX_VALID(fbo_index) (fbo_index < MAX_FBO_AMOUNT)

#define DEFAULT_FBO (0)


uint32_t fbos_amount = 0;
uint32_t current_fbo = INVALID_FBO_INDEX;
fbo_t* fbos_list[_MAX_FBO_AMOUNT_];
const uint32_t MAX_FBO_AMOUNT = _MAX_FBO_AMOUNT_;


static err_t gl_generate_framebuffer(uint32_t* out_gl_fbo) {
    err_t err = NO_ERROR;

    CHECK(out_gl_fbo != NULL);

    DEBUG_CHECK_NO_GL_ERROR();
    glGenFramebuffers(1, out_gl_fbo);
    DEBUG_CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t gl_bind_framebuffer(uint32_t gl_fbo) {
    err_t err = NO_ERROR;

    DEBUG_CHECK_NO_GL_ERROR();
    glBindFramebuffer(GL_FRAMEBUFFER, gl_fbo);
    DEBUG_CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t gl_delete_texture(uint32_t gl_texture) {
    err_t err = NO_ERROR;

    DEBUG_CHECK_NO_GL_ERROR();
    glDeleteTextures(1, &gl_texture);
    CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t gl_generate_texture_2d(uint32_t* out_gl_texture,
                                    GLint internal_format, GLint format, GLenum type,
                                    uint32_t width, uint32_t height,
                                    GLenum attachment,
                                    texture_param_setter_callback_t param_setter_callback) {
    err_t err = NO_ERROR;
    uint32_t gl_texture = INVALID_FBO_ATTACHMENT_VALUE;

    CHECK(out_gl_texture != NULL);
    *out_gl_texture = INVALID_FBO_ATTACHMENT_VALUE;
    
    CHECK_NO_GL_ERROR();

    glGenTextures(1, &gl_texture);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    DEBUG_CHECK_NO_GL_ERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, NULL);
    DEBUG_CHECK_NO_GL_ERROR();
    
    // default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    DEBUG_CHECK_NO_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    DEBUG_CHECK_NO_GL_ERROR();

    // user set texture parameters
    if (param_setter_callback != NULL) {
        RETHROW_IF_ERROR(param_setter_callback());
    }
    DEBUG_CHECK_NO_GL_ERROR();

    // attach to current frame buffer object
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, gl_texture, 0);  
    
    CHECK_NO_GL_ERROR();

    *out_gl_texture = gl_texture;
    gl_texture = INVALID_FBO_ATTACHMENT_VALUE;

cleanup:
    if (IS_ERROR(err) && gl_texture != INVALID_FBO_ATTACHMENT_VALUE) {
        gl_delete_texture(gl_texture);
        gl_texture = INVALID_FBO_ATTACHMENT_VALUE;
    }

    return err;
}

static err_t gl_delete_renderbuffer(uint32_t gl_rbo) {
    err_t err = NO_ERROR;

    DEBUG_CHECK_NO_GL_ERROR();
    glDeleteRenderbuffers(1, &gl_rbo);
    CHECK_NO_GL_ERROR();


cleanup:
    return err;
}

static err_t gl_generate_renderbuffer(  uint32_t* out_gl_rbo,
                                        GLenum internalformat,
                                        uint32_t width, uint32_t height,
                                        GLenum attachment) {
    err_t err = NO_ERROR;
    uint32_t gl_rbo = INVALID_FBO_ATTACHMENT_VALUE;

    CHECK(out_gl_rbo != NULL);
    *out_gl_rbo = INVALID_FBO_ATTACHMENT_VALUE;

    CHECK_NO_GL_ERROR();

    glGenRenderbuffers(1, &gl_rbo);
    DEBUG_CHECK_NO_GL_ERROR();

    glBindRenderbuffer(GL_RENDERBUFFER, gl_rbo);
    DEBUG_CHECK_NO_GL_ERROR();

    glRenderbufferStorage(GL_RENDERBUFFER, internalformat, width, height);
    DEBUG_CHECK_NO_GL_ERROR();

    // attach to current frame buffer object
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, gl_rbo);
    
    CHECK_NO_GL_ERROR();

    *out_gl_rbo = gl_rbo;
    gl_rbo = INVALID_FBO_ATTACHMENT_VALUE;

cleanup:
    if (IS_ERROR(err) && gl_rbo != INVALID_FBO_ATTACHMENT_VALUE) {
        gl_delete_renderbuffer(gl_rbo);
        gl_rbo = INVALID_FBO_ATTACHMENT_VALUE;
    }

    return err;
}

static err_t gl_check_framebuffer_status(GLenum* out_status) {
    err_t err = NO_ERROR;
    GLenum status = GL_FRAMEBUFFER_UNDEFINED;

    CHECK(out_status != NULL);
    *out_status = GL_FRAMEBUFFER_UNDEFINED;

    DEBUG_CHECK_NO_GL_ERROR();
    status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    DEBUG_CHECK_NO_GL_ERROR();

    *out_status = status;

cleanup:
    return err;
}

err_t check_fbo(fbo_t* fbo) {
    err_t err = NO_ERROR;

    CHECK(fbo != NULL);
    CHECK(IS_FBO_INDEX_VALID(fbo->fbo_index));
    CHECK(fbos_list[fbo->fbo_index] == fbo);

cleanup:
    return err;
}

static err_t gl_delete_framebuffer(uint32_t gl_fbo) {
    err_t err = NO_ERROR;

    CHECK(gl_fbo != INVALID_GL_FBO);

    DEBUG_CHECK_NO_GL_ERROR();
    glDeleteFramebuffers(1, &gl_fbo);
    CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static void clean_fbo(fbo_t* fbo) {
    if (fbo != NULL) {
        switch (fbo->color_attachment_type) {
        case TEXTURE_COLOR_FBO_ATTACHMENT:
            gl_delete_texture(fbo->color_attchment.gl_texture);
            break;
        case RENDER_BUFFER_OBJECT_FBO_ATTACHMENT:
            gl_delete_renderbuffer(fbo->color_attchment.gl_rbo);
            break;
        case NO_COLOR_FBO_ATTACHMENT:
            break;
        }
        fbo->color_attchment.value = INVALID_FBO_ATTACHMENT_VALUE;
    
        switch (fbo->secondery_attachment_type) {
        case DEPTH_STENCIL_TEXTURE_SECONDERY_FBO_ATTACHMENT:
        case DEPTH_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT:
        case STENCIL_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT:
            gl_delete_texture(fbo->secondery_attachment.gl_texture);
            break;
        case DEPTH_STENCIL_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT:
        case DEPTH_ONLY_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT:
        case STENCIL_ONLY_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT:
            gl_delete_renderbuffer(fbo->secondery_attachment.gl_rbo);
            break;
        case NO_SECONDERY_FBO_ATTACHMENT:
            break;
        }
        fbo->secondery_attachment.value = INVALID_FBO_ATTACHMENT_VALUE;
        
        gl_delete_framebuffer(fbo->gl_fbo);
        fbo->gl_fbo = INVALID_GL_FBO;

        free(fbo);
        fbo = NULL;
    }
}

err_t create_fbo_ext_param( fbo_t** out_fbo,
                            uint32_t width, uint32_t height,
                            color_fbo_attachment_type_t color_attachment_type,
                            uint32_t color_attachment_format,
                            texture_param_setter_callback_t texture_color_attachment_param_setter_callback,
                            secondery_fbo_attachment_type_t secondery_attachment_type,
                            texture_param_setter_callback_t texture_secondery_attachment_param_setter_callback) {
    err_t err = NO_ERROR;
    fbo_t* fbo = NULL;
    uint32_t fbo_index = INVALID_FBO_INDEX;
    uint32_t last_fbo = current_fbo;
    GLenum fbo_status = GL_FRAMEBUFFER_UNDEFINED;

    CHECK(out_fbo != NULL);
    *out_fbo = NULL;

    CHECK(fbos_amount < MAX_FBO_AMOUNT);
    
    for (uint32_t i = 0; i < MAX_FBO_AMOUNT; i++) {
        if (fbos_list[i] == NULL) {
            fbo_index = i;
            break;
        }
    }
    CHECK(IS_FBO_INDEX_VALID(fbo_index));

    fbo = malloc(sizeof(fbo_t));
    CHECK(fbo != NULL);

    UNCONSTIFY(uint32_t, fbo->fbo_index) = INVALID_FBO_INDEX;
    fbo->gl_fbo = INVALID_GL_FBO;
    fbo->width = width;
    fbo->height = height;
    fbo->color_attachment_type = color_attachment_type;
    fbo->color_attchment.value = INVALID_FBO_ATTACHMENT_VALUE;
    fbo->color_attachment_format = color_attachment_format;
    fbo->secondery_attachment_type = secondery_attachment_type;
    fbo->secondery_attachment.value = INVALID_FBO_ATTACHMENT_VALUE;

    CHECK_NO_GL_ERROR();

    RETHROW_IF_ERROR(gl_generate_framebuffer(&fbo->gl_fbo));
    RETHROW_IF_ERROR(gl_bind_framebuffer(fbo->gl_fbo));

    switch (color_attachment_type) {
    case TEXTURE_COLOR_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_texture_2d(
            &fbo->color_attchment.gl_texture,
            color_attachment_format, color_attachment_format, GL_UNSIGNED_BYTE,
            width, height,
            GL_COLOR_ATTACHMENT0,
            texture_color_attachment_param_setter_callback
        ));
        break;
    
    case RENDER_BUFFER_OBJECT_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_renderbuffer(
            &fbo->color_attchment.gl_rbo,
            color_attachment_format,
            width, height,
            GL_COLOR_ATTACHMENT0
        ));
        break;

    case NO_COLOR_FBO_ATTACHMENT:
        fbo->color_attchment.value = INVALID_FBO_ATTACHMENT_VALUE;
        break;
    
    default:
        THROW();
    }

    // secondery attachment
    switch (secondery_attachment_type) {
    case DEPTH_STENCIL_TEXTURE_SECONDERY_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_texture_2d(
            &fbo->secondery_attachment.gl_texture,
            GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8,
            width, height,
            GL_DEPTH_STENCIL_ATTACHMENT,
            texture_secondery_attachment_param_setter_callback
        ));
        break;
    case DEPTH_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_texture_2d(
            &fbo->secondery_attachment.gl_texture,
            GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT,
            width, height,
            GL_DEPTH_ATTACHMENT,
            texture_secondery_attachment_param_setter_callback
        ));
        break;
    case STENCIL_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_texture_2d(
            &fbo->secondery_attachment.gl_texture,
            GL_STENCIL_INDEX, GL_STENCIL_INDEX, GL_UNSIGNED_INT,
            width, height,
            GL_STENCIL_ATTACHMENT,
            texture_secondery_attachment_param_setter_callback
        ));
        break;

    case DEPTH_STENCIL_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_renderbuffer(
            &fbo->secondery_attachment.gl_rbo,
            GL_DEPTH24_STENCIL8,
            width, height,
            GL_DEPTH_STENCIL_ATTACHMENT
        ));
        break;
    case DEPTH_ONLY_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_renderbuffer(
            &fbo->secondery_attachment.gl_rbo,
            GL_DEPTH_COMPONENT,
            width, height,
            GL_DEPTH_ATTACHMENT
        ));
        break;
    case STENCIL_ONLY_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT:
        RETHROW_IF_ERROR(gl_generate_renderbuffer(
            &fbo->secondery_attachment.gl_rbo,
            GL_STENCIL_INDEX,
            width, height,
            GL_STENCIL_ATTACHMENT
        ));
        break;

    case NO_SECONDERY_FBO_ATTACHMENT:
        fbo->secondery_attachment.value = INVALID_FBO_ATTACHMENT_VALUE;
        break;

    default:
        THROW();
    }

    RETHROW_IF_ERROR(gl_check_framebuffer_status(&fbo_status));
    CHECK(fbo_status == GL_FRAMEBUFFER_COMPLETE);

    // rebind last frame buffer here
    if (last_fbo == INVALID_FBO_INDEX) {
        RETHROW_IF_ERROR(use_default_fbo());
    } else {
        RETHROW_IF_ERROR(use_fbo(fbos_list[last_fbo]));
    }

    CHECK_NO_GL_ERROR();

    // append fbo to fbos_list
    UNCONSTIFY(uint32_t, fbo->fbo_index) = fbo_index;
    fbos_list[fbo_index] = fbo;
    fbos_amount += 1;

    *out_fbo = fbo;

cleanup:
    if (IS_ERROR(err) && fbo != NULL) {
        clean_fbo(fbo);
        fbo = NULL;
    }

    return err;
}

err_t create_fbo(   fbo_t** out_fbo,
                    uint32_t width, uint32_t height,
                    color_fbo_attachment_type_t color_attachment_type,
                    GLint color_attachment_format,
                    secondery_fbo_attachment_type_t secondery_attachment_type) {
    err_t err = NO_ERROR;
    
    RETHROW_IF_ERROR(create_fbo_ext_param(
        out_fbo,
        width, height,
        color_attachment_type,
        color_attachment_format,
        NULL,
        secondery_attachment_type,
        NULL
    ));

cleanup:
    return err;
}

err_t use_default_fbo() {
    err_t err = NO_ERROR;

    current_fbo = INVALID_FBO_INDEX;
    RETHROW_IF_ERROR(gl_bind_framebuffer(DEFAULT_FBO));
    RETHROW_IF_ERROR(update_viewport_by_current_camera());

cleanup:
    return err;
}

err_t use_fbo(fbo_t* fbo) {
    err_t err = NO_ERROR;

    CHECK_FBO(fbo);

    if (current_fbo != fbo->fbo_index) {
        current_fbo = fbo->fbo_index;
        RETHROW_IF_ERROR(gl_bind_framebuffer(fbo->gl_fbo));
    }

    RETHROW_IF_ERROR(update_viewport_by_current_camera());

cleanup:
    return err;
}

static err_t bind_texture_to_uniform(uint32_t gl_texture, int32_t uniform_location, uint8_t texture_num) {
    err_t err = NO_ERROR;

    DEBUG_CHECK_NO_GL_ERROR();
    
    glUniform1i(uniform_location, texture_num);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glActiveTexture(GL_TEXTURE0 + texture_num);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    DEBUG_CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

err_t bind_fbo_color_texture(fbo_t* fbo, int32_t uniform_location, uint8_t texture_num) {
    err_t err = NO_ERROR;

    CHECK(fbo != NULL);
    CHECK(fbo->color_attachment_type == TEXTURE_COLOR_FBO_ATTACHMENT);

    RETHROW_IF_ERROR(bind_texture_to_uniform(fbo->color_attchment.gl_texture, uniform_location, texture_num));

cleanup:
    return err;
}

err_t bind_fbo_depth_stencil_texture(fbo_t* fbo, int32_t uniform_location, uint8_t texture_num) {
    err_t err = NO_ERROR;

    CHECK(fbo != NULL);
    CHECK(
        fbo->secondery_attachment_type == DEPTH_STENCIL_TEXTURE_SECONDERY_FBO_ATTACHMENT ||
        fbo->secondery_attachment_type == DEPTH_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT ||
        fbo->secondery_attachment_type == STENCIL_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT
    );

    RETHROW_IF_ERROR(bind_texture_to_uniform(fbo->secondery_attachment.gl_texture, uniform_location, texture_num));

cleanup:
    return err;
}

err_t destroy_fbo(fbo_t* fbo) {
    err_t err = NO_ERROR;
    
    CHECK_FBO(fbo);
    CHECK(current_fbo != fbo->fbo_index);
    CHECK(fbos_amount > 0);

    fbos_list[fbo->fbo_index] = NULL;
    fbos_amount -= 1;
    
cleanup:
    clean_fbo(fbo);
    
    return err;
}

void clean_fbos() {
    DEBUG_PRINT("cleaning %u fbos\n", fbos_amount);
    
    for (uint32_t i = 0; i < MAX_FBO_AMOUNT; i++) {
        if (fbos_list[i] != NULL) {
            destroy_fbo(fbos_list[i]);
        }
    }
    
    if (fbos_amount != 0) {
        DEBUG_PRINT("leaked a fbo\n");
    }

    fbos_amount = 0;
}
