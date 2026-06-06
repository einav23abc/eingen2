#ifndef __FRAME_BUFFER_OBJECTS_H__
#define __FRAME_BUFFER_OBJECTS_H__

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#include "codeflow.h"
#include "../glad/glad.h"
#include "textures/textures.h"


#define INVALID_FBO_INDEX (UINT32_MAX)
#define INVALID_GL_FBO (UINT32_MAX)
#define INVALID_FBO_ATTACHMENT_VALUE (UINT32_MAX)

#define CHECK_FBO(fbo_ptr) RETHROW_IF_ERROR(check_fbo(fbo_ptr))


typedef enum color_fbo_attachment_type_e {
    NO_COLOR_FBO_ATTACHMENT,
    TEXTURE_COLOR_FBO_ATTACHMENT,
    RENDER_BUFFER_OBJECT_FBO_ATTACHMENT,
} color_fbo_attachment_type_t;

typedef enum secondery_fbo_attachment_type_e {
    NO_SECONDERY_FBO_ATTACHMENT,
    DEPTH_STENCIL_TEXTURE_SECONDERY_FBO_ATTACHMENT,
    DEPTH_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT,
    STENCIL_ONLY_TEXTURE_SECONDERY_FBO_ATTACHMENT,
    DEPTH_STENCIL_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT,
    DEPTH_ONLY_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT,
    STENCIL_ONLY_RENDER_BUFFER_OBJECT_SECONDERY_FBO_ATTACHMENT,
} secondery_fbo_attachment_type_t;

typedef struct {
    const uint32_t fbo_index;

    uint32_t gl_fbo;

    uint32_t width;
    uint32_t height;

    color_fbo_attachment_type_t color_attachment_type;
    union {
        uint32_t value;
        uint32_t gl_texture;
        uint32_t gl_rbo;
    } color_attchment;
    uint32_t color_attachment_format;

    secondery_fbo_attachment_type_t secondery_attachment_type;
    union {
        uint32_t value;
        uint32_t gl_texture;
        uint32_t gl_rbo;
    } secondery_attachment;
} fbo_t;


err_t check_fbo(fbo_t* fbo);

/* Creates a frame-buffer-object (fbo).
 * 
 * The memory delocation of this fbo is handled by the engine (by calling `clean_fbos()`).
 *
 * @param out_fbo The create fbo
 * @param width The width of the fbo's textures/render-objects.
 * @param height The height of the fbo's textures/render-objects.
 * @param color_attachment_type Sets how the fbo will store color data:
 * `0` - no color attachment.
 * `1` - texture.
 * `2` - render buffer object.
 * @param texture_color_attachment_format Only relevent for texture color attachment. `GL_RGB`, `GL_RGBA`, etc.
 * @param texture_color_attachment_param_setter_callback Only relevent for texture color attachment. Null can be passed for no function.
 * A pointer to a function that will be called after the color texture is binded and before it is attached to the fbo.
 * Should be used to call `glTexParameter`. Null can be passed instead for no function.
 * @param secondery_attachment_type Sets how the fbo will store depth and stencil data:
 * `0` - no depth and stencil attachment.
 * `1` - texture.
 * `2` - texture, only depth.
 * `3` - texture, only stencil.
 * `4` - render buffer object.
 * `5` - render buffer object, only depth.
 * `6` - render buffer object, only stencil.
 * @param texture_secondery_attachment_param_setter_callback Only relevent for texture depth-stencil attachments. Null can be passed for no function.
 * 
 * @returns err_t
 */
err_t create_fbo_ext_param( fbo_t** out_fbo,
                            uint32_t width, uint32_t height,
                            color_fbo_attachment_type_t color_attachment_type,
                            uint32_t color_attachment_format,
                            texture_param_setter_callback_t texture_color_attachment_param_setter_callback,
                            secondery_fbo_attachment_type_t secondery_attachment_type,
                            texture_param_setter_callback_t texture_secondery_attachment_param_setter_callback);

/* Creates a frame-buffer-object (fbo).
 * 
 * The destruction of this fbo is handled by the engine (by calling `clean_fbos()`).
 *
 * @param out_fbo The create fbo
 * @param width The width of the frame buffer object's textures/render objects.
 * @param height The height of the frame buffer object's textures/render objects.
 * @param color_attachment_type Sets how the fbo will store color data:
 * `0` - no color attachment.
 * `1` - texture.
 * `2` - render buffer object.
 * @param color_attachment_format Only relevent for texture color attachment. `GL_RGB`, `GL_RGBA`, etc. Null can be passed for no function.
 * A pointer to a function that will be called after the color texture is binded and before it is attached to the frame buffer object.
 * Should be used to call `glTexParameter`. Null can be passed instead for no function.
 * @param secondery_attachment_type Sets how the fbo will store depth and stencil data:
 * `0` - no depth and stencil attachment.
 * `1` - texture.
 * `2` - texture, only depth.
 * `3` - texture, only stencil.
 * `4` - render buffer object.
 * `5` - render buffer object, only depth.
 * `6` - render buffer object, only stencil.
 * 
 * @returns err_t
 */
err_t create_fbo(   fbo_t** out_fbo,
                    uint32_t width, uint32_t height,
                    color_fbo_attachment_type_t color_attachment_type,
                    GLint color_attachment_format,
                    secondery_fbo_attachment_type_t secondery_attachment_type);

/* Sets the fbo that will be used for all draw functions to the defaule fbo (the window), Until a diffrent fbo is used.
 *
 * Updates the fbo's viewport by calling `camera_update_fbo_viewport()` with the `current_camera`,
 */
err_t use_default_fbo();

/* Sets the fbo that will be used for all draw functions, Until a diffrent fbo is used.
 *
 * Updates the fbo's viewport by calling `camera_update_fbo_viewport()` with the `current_camera`,
 * 
 * @param fbo The fbo to use.
 */
err_t use_fbo(fbo_t* fbo);

err_t bind_fbo_color_texture(fbo_t* fbo, int32_t uniform_location, uint8_t texture_num);
err_t bind_fbo_depth_stencil_texture(fbo_t* fbo, int32_t uniform_location, uint8_t texture_num);

// * Will not destroy the `current_fbo`. If you wish to do so, call `use_fbo()` with a diffrent fbo first.
// * @param fbo the fbo to destroy. This fbo will not be usable after destruction.
// * @returns err_t
err_t destroy_fbo(fbo_t* fbo);

/* Called by the engine when exiting.
 * Destroys all created fbos.
 */
void clean_fbos();

#endif // __FRAME_BUFFER_OBJECTS_H__
