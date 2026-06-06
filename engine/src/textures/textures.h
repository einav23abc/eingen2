#ifndef __TEXTURES_H__
#define __TEXTURES_H__

#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "../glad/glad.h"
#include "codeflow.h"


#define INVALID_TEXTURE_INDEX (UINT32_MAX)
#define INVALID_GL_TEXTURE (UINT32_MAX)

#define CHECK_TEXTURE(texture_ptr) RETHROW_IF_ERROR(check_texture(texture_ptr))


typedef err_t (*texture_param_setter_callback_t)(void);


typedef struct {
    const uint32_t texture_index;

    uint32_t gl_texture;
} texture_t;


err_t gl_delete_texture(uint32_t gl_texture);

err_t gl_generate_texture_2d(uint32_t* out_gl_texture,
                             GLint internal_format, GLint format, GLenum type,
                             uint32_t width, uint32_t height,
                             const void* data,
                             texture_param_setter_callback_t param_setter_callback);


err_t check_texture(texture_t* texture);

/**
 * @brief Create a texture from a SDL_Surface. 
 * 
 * The memory delocation of this texture is handled by the engine (by calling `clean_textures`).
 *
 * @param texture_load_surface The SDL_Surface to create a texture from.
 * 
 * @param param_func A pointer to a function that will be called after the texture is binded and before the image is loaded.
 * Should be used to call `glTexParameter`. Null can be passed for no function.
 * 
 * @returns A pointer to an allocated texture or `NULL` on failure.
 */
err_t load_texture_from_surface_ext_params(texture_t** out_texture, SDL_Surface* texture_load_surface, texture_param_setter_callback_t param_setter_callback);

/**
 * @brief Create a texture from a SDL_Surface.
 * 
 * The memory delocation of this texture is handled by the engine (by calling `clean_textures`).
 *
 * @param texture_load_surface The SDL_Surface to create a texture from.
 * 
 * @returns A pointer to an allocated texture or `NULL` on failure.
 */
err_t load_texture_from_surface(texture_t** out_texture, SDL_Surface* texture_load_surface);

/**
 * @brief Create a texture from an image file. 
 * 
 * The memory delocation of this texture is handled by the engine (by calling `clean_textures`).
 *
 * @param file_path The path to the texture's image.
 * 
 * @param param_func A pointer to a function that will be called after the texture is binded and before the image is loaded.
 * Should be used to call `glTexParameter`. Null can be passed for no function.
 * 
 * @returns A pointer to an allocated texture or `NULL` on failure.
 */
err_t load_texture_ext_params(texture_t** out_texture, const char* file_path, texture_param_setter_callback_t param_setter_callback);

/**
 * @brief Create a texture from an image file. 
 * 
 * The memory delocation of this texture is handled by the engine (by calling `clean_textures`).
 *
 * @param file_path The path to the texture's image.
 * 
 * @returns A pointer to an allocated texture or `NULL` on failure.
 */
err_t load_texture(texture_t** out_texture, const char* file_path);

err_t bind_gl_texture_to_uniform(uint32_t gl_texture, int32_t uniform_location, uint8_t texture_num);

/**
 * @brief Bind a texture to a uniform for use in a shader.
 *
 * The texture will be binded to an index `texture_num` and the uniform will
 * be set to `GL_TEXTUREx` where x is the `texture_num`. Due to this, every texture
 * binded to the same shader during the same time must have a unique `texture_num`.
 * 
 * @param texture The texture to use in the shader.
 * 
 * @param uniform The location of the uniform.
 * 
 * @param texture_num The index assigned to the texture.
 */
err_t bind_texture_to_uniform(texture_t* texture, int32_t uniform_location, uint8_t texture_num);

/**
 * @brief Destroys a texture.
 *
 * The texture will not be usable after destruction.
 *
 * @param texture the texture to destroy.
 */
err_t destroy_texture(texture_t* texture);

/* Called by the engine when exiting.
 * Destroys all created textures.
 */
void clean_textures();

/**
 * @brief Save an SDL_Surface into a .c file.
 *
 * Can be used to load a texture without having to load from a file.
 * This can be done by calling `load_texture_from_surface` or `load_texture_from_surface_ext_params`.
 * 
 * Unlike surfaces created by SDL's functions, This surface does not need to be freed using `SDL_FreeSurface`.
 * 
 * @param image_file_path The path to the texture's image.
 * 
 * @param name The name that will be given to the surface. This will be `SDL_Surface* name`.
 * 
 * @param surface_file_path The path to the c file where the surface will be saved.
 */
void save_surface_to_c_file(const char* image_file_path, const char* name, const char* c_file_path);

#endif // __TEXTURES_H__
