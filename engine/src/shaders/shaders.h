#ifndef __SHADERS_H__
#define __SHADERS_H__

#include <SDL2/SDL.h>
#include <stdint.h>
#include <stdio.h>
#include <limits.h>

#include "codeflow.h"
#include "../glad/glad.h"
#include "cameras/cameras.h"


#define u_camera_wvp_mat_loc u_camera_world_view_projection_matrix_loc

#define CHECK_SHADER(shader_ptr) RETHROW_IF_ERROR(check_shader(shader_ptr))


typedef struct {
    const uint32_t shader_index;
    
    uint8_t is_initialized : 1;

    uint32_t gl_program;
    uint32_t vert_gl_shader;
    uint32_t frag_gl_shader;

    // default uniforms
    int32_t u_texture_loc;
    int32_t u_camera_world_view_projection_matrix_loc;
    int32_t u_joint_matrices_loc;
    int32_t u_instanced_drawing_float_data_loc;
    int32_t u_instanced_drawing_uint_data_loc;

    // user defined uniforms
    int32_t* uniform_locations;
    // user defined attributes
    int32_t* attribute_locations;

    // the camera index the u_camera_world_view_projection_matrix_loc uniform was last updated by
    uint32_t wvp_mat_camera_index;
} shader_t;



err_t check_shader(shader_t* shader);

/* Creates a shader.
 * 
 * The memory delocation of this shader is handled by the engine (by calling `clean_shaders()`).
 * 
 * @param out_shader, a pointer to hold the created shader.
 *
 * @param vert_shader_str A pointer to the vertex shader string.
 * @param frag_shader_str A pointer to the fragment shader string.
 * 
 * @param attribute_names A string with all the shader's attribute names, Seperated by `\0`. For example: `"in_vertex_position\0in_vertex_texcoord\0in_vertex_normal"`.
 * @param attributes_count The amount of attributes in `attribute_names`.
 * @param uniform_names A string with all the shader's uniform names, Seperated by `\0`. For example: `"u_position\0u_scale\0u_quat_rotation"`.
 * @param uniforms_count The amount of uniforms in `uniform_names`.
 * 
 * @returns err_t
*/
err_t create_shader(shader_t** const out_shader,
                    const char* vert_shader_str, const char* frag_shader_str,
                    const char* attribute_names , uint32_t attributes_count,
                    const char* uniform_names, uint32_t uniforms_count);

/* Creates a shader.
 * 
 * The memory delocation of this shader is handled by the engine (by calling `clean_shaders()`).
 *
 * @param out_shader, a pointer to hold the created shader.
 * 
 * @param vert_shader_file_path A string of the vertex shader file's path.
 * @param frag_shader_file_path A string of the fragment shader file's path.
 * 
 * @param attribute_names A string with all the shader's attribute names, Seperated by `\0`. For example: `"in_vertex_position\0in_vertex_texcoord\0in_vertex_normal"`.
 * @param attributes_count The amount of attributes in `attribute_names`.
 * @param uniform_names A string with all the shader's uniform names, Seperated by `\0`. For example: `"u_position\0u_scale\0u_quat_rotation"`.
 * @param uniforms_count The amount of uniforms in `uniform_names`.
 * 
 * @returns err_t
*/
err_t create_shader_from_files( shader_t** const out_shader,
                                const char* vert_shader_file_path, const char* frag_shader_file_path,
                                const char* attribute_names , uint32_t attributes_count,
                                const char* uniform_names, uint32_t uniforms_count);

/* Sets the `current_shader` and uses it for all draw functions until a diffrent shader is used.
 *
 * Updates the shader's `u_camera_world_view_projection_matrix` uniform by calling `update_shader_uniforms_by_current_camera()`,
 * Unless that uniform is already set to the current camera, Indicated by the `shader`'s `wvp_mat_camera_index`.
 * 
 * @param shader The shader to use, if the `shader`'s index is `current_shader` the function has no effect.
*/
err_t use_shader(shader_t* shader);

err_t get_current_shader(shader_t** out_current_shader);

/**
 * Sets a shader's `u_camera_world_view_projection_matrix` uniform to the value of a camera's `world_view_projection_matrix`.
 * 
 * Gets called automaticly for the current camera and current shader by `use_shader()` via `update_shader_uniforms_by_current_camera` and
 * `camera_use()` via `update_current_shader_uniforms_by_camera`, so generally there is no reason to call this function.
 */
err_t update_shader_uniforms_by_camera(shader_t* shader, camera_t* camera);

err_t update_shader_uniforms_by_current_camera(shader_t* shader);
err_t update_current_shader_uniforms_by_camera(camera_t* camera);

/* Destroys a shader.
 *
 * The shader will not be usable after destruction.
 *
 * Will not destroy the `current_shader`, if you wish to do so, call `use_shader()` with a differnt shader first.
 * @param shader the shader to destroy.
 * @returns err_t
*/
err_t destroy_shader(shader_t* shader);

/* Called by the engine when exiting.
 * Destroys all created shaders.
*/
void clean_shaders();

#endif // __SHADERS_H__
