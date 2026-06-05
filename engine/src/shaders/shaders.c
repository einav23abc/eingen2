#include "shaders.h"

#include "macros.h"
#include "gl_macros.h"
#include "codeflow.h"
#include "../utils/utils.h"
#include "../cameras/cameras.h"


#ifndef _MAX_SHADERS_AMOUNT
#define _MAX_SHADERS_AMOUNT_ (128)
#endif

#define INVALID_SHADER_INDEX (UINT32_MAX)
#define IS_SHADER_INDEX_VALID(shader_index) (shader_index < MAX_SHADERS_AMOUNT)

#define INVALID_GL_SHADER (0)
#define INVALID_GL_PROGRAM (0)
#define INVALID_GL_ATTRIBUTE_LOCATION (-1)
#define INVALID_GL_UNIFORM_LOCATION (-1)

#define GL_ERROR_MESSAGE_BUFFER_SIZE (512)


uint32_t shaders_amount = 0;
uint32_t current_shader = INVALID_SHADER_INDEX;
shader_t* shaders_list[_MAX_SHADERS_AMOUNT_];
const uint32_t MAX_SHADERS_AMOUNT = _MAX_SHADERS_AMOUNT_;



static err_t gl_create_shader(uint32_t* const out_gl_shader, GLenum shader_type) {
    err_t err = NO_ERROR;

    CHECK(out_gl_shader != NULL);

    DEBUG_CHECK_NO_GL_ERROR();
    *out_gl_shader = glCreateShader(shader_type);
    DEBUG_CHECK_NO_GL_ERROR();
    CHECK(*out_gl_shader != INVALID_GL_PROGRAM);

cleanup:
    return err;
}

static err_t gl_shader_source(uint32_t gl_shader, const char* shader_source_string) {
    err_t err = NO_ERROR;

    CHECK(gl_shader != INVALID_GL_SHADER);
    CHECK(shader_source_string != NULL);

    DEBUG_CHECK_NO_GL_ERROR();
    glShaderSource(gl_shader, 1, &shader_source_string, NULL);
    DEBUG_CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t print_gl_shader_info_log(uint32_t gl_shader) {
    err_t err = NO_ERROR;
    char error_message_buffer[GL_ERROR_MESSAGE_BUFFER_SIZE] = {0};

    CHECK(gl_shader != INVALID_GL_SHADER);

    glGetShaderInfoLog(gl_shader, sizeof(error_message_buffer), NULL, error_message_buffer);
    error_message_buffer[sizeof(error_message_buffer) - 1] = '\0';
    DEBUG_PRINT("gl_shader %d info log:\n\"%s\"\n", gl_shader, error_message_buffer);

    CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t gl_compile_shader(uint32_t gl_shader) {
    err_t err = NO_ERROR;
    int32_t status = GL_FALSE;

    CHECK(gl_shader != INVALID_GL_SHADER);
    
    CHECK_NO_GL_ERROR();
    glCompileShader(gl_shader);
    CHECK_NO_GL_ERROR();
    glGetShaderiv(gl_shader, GL_COMPILE_STATUS, &status);
    CHECK_NO_GL_ERROR();
    CHECK(status == GL_TRUE);

cleanup:
    if (IS_ERROR(err) && gl_shader != INVALID_GL_SHADER) {
        print_gl_shader_info_log(gl_shader);
    }

    return err;
}

static err_t gl_delete_shader(uint32_t gl_shader) {
    err_t err = NO_ERROR;

    CHECK(gl_shader != INVALID_GL_SHADER);
    
    DEBUG_CHECK_NO_GL_ERROR();
    glDeleteShader(gl_shader);
    CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t gl_create_program(uint32_t* const out_gl_program) {
    err_t err = NO_ERROR;

    CHECK(out_gl_program != NULL);

    DEBUG_CHECK_NO_GL_ERROR();
    *out_gl_program = glCreateProgram();
    DEBUG_CHECK_NO_GL_ERROR();
    CHECK(*out_gl_program != INVALID_GL_PROGRAM);

cleanup:
    return err;
}

static err_t gl_attach_shader(uint32_t gl_program, uint32_t gl_shader) {
    err_t err = NO_ERROR;

    CHECK(gl_program != INVALID_GL_PROGRAM);
    CHECK(gl_shader != INVALID_GL_SHADER);
    
    DEBUG_CHECK_NO_GL_ERROR();
    glAttachShader(gl_program, gl_shader);
    DEBUG_CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t print_gl_program_info_log(uint32_t gl_program) {
    err_t err = NO_ERROR;
    char error_message_buffer[GL_ERROR_MESSAGE_BUFFER_SIZE] = {0};

    CHECK(gl_program != INVALID_GL_PROGRAM);

    glGetProgramInfoLog(gl_program, sizeof(error_message_buffer), NULL, error_message_buffer);    
    error_message_buffer[sizeof(error_message_buffer) - 1] = '\0';
    DEBUG_PRINT("gl_program %d info log:\n\"%s\"\n", gl_program, error_message_buffer);

    CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

static err_t gl_link_program(uint32_t gl_program) {
    err_t err = NO_ERROR;
    int32_t status = GL_FALSE;

    CHECK_NO_GL_ERROR();
    glLinkProgram(gl_program);
    CHECK_NO_GL_ERROR();
    glGetProgramiv(gl_program, GL_LINK_STATUS, &status);
    CHECK_NO_GL_ERROR();
    CHECK(status == GL_TRUE);

cleanup:
    if (IS_ERROR(err) && gl_program != INVALID_GL_PROGRAM) {
        print_gl_program_info_log(gl_program);
    }

    return err;
}

static err_t gl_get_attribute_location(int32_t* out_attribute_location, uint32_t gl_program, const char* attribute_name) {
    err_t err = NO_ERROR;
    int32_t attribute_location = INVALID_GL_ATTRIBUTE_LOCATION;

    CHECK(out_attribute_location != NULL);
    *out_attribute_location = INVALID_GL_ATTRIBUTE_LOCATION;

    CHECK(gl_program != INVALID_GL_PROGRAM);
    CHECK(attribute_name != NULL);

    DEBUG_CHECK_NO_GL_ERROR();
    attribute_location = glGetAttribLocation(gl_program, attribute_name);
    CHECK_NO_GL_ERROR();

    *out_attribute_location = attribute_location;

cleanup:
    return err;
}

static err_t gl_get_uniform_location(int32_t* out_uniform_location, uint32_t gl_program, const char* uniform_name) {
    err_t err = NO_ERROR;
    int32_t uniform_location = INVALID_GL_UNIFORM_LOCATION;

    CHECK(out_uniform_location != NULL);
    *out_uniform_location = INVALID_GL_UNIFORM_LOCATION;

    CHECK(gl_program != INVALID_GL_PROGRAM);
    CHECK(uniform_name != NULL);

    DEBUG_CHECK_NO_GL_ERROR();
    uniform_location = glGetUniformLocation(gl_program, uniform_name);
    CHECK_NO_GL_ERROR();

    *out_uniform_location = uniform_location;

cleanup:
    return err;
}

static err_t gl_use_program(uint32_t gl_program) {
    err_t err = NO_ERROR;

    CHECK(gl_program != INVALID_GL_PROGRAM);

    DEBUG_CHECK_NO_GL_ERROR();
    glUseProgram(gl_program);
    DEBUG_CHECK_NO_GL_ERROR();

cleanup:
    return err;
}


err_t check_shader(shader_t* shader) {
    err_t err = NO_ERROR;

    CHECK(shader != NULL);
    CHECK(IS_SHADER_INDEX_VALID(shader->shader_index));
    CHECK(shaders_list[shader->shader_index] == shader);
    CHECK(shader->is_initialized);

cleanup:
    return err;
}

static err_t reset_shader(shader_t* shader) {
    err_t err = NO_ERROR;

    CHECK(shader != NULL);

    UNCONSTIFY(uint32_t, shader->shader_index) = INVALID_SHADER_INDEX;
    shader->is_initialized = 0;
    
    shader->gl_program = INVALID_GL_PROGRAM;
    shader->vert_gl_shader = INVALID_GL_SHADER;
    shader->frag_gl_shader = INVALID_GL_SHADER;
    
    shader->u_texture_loc = INVALID_GL_UNIFORM_LOCATION;
    shader->u_camera_world_view_projection_matrix_loc = INVALID_GL_UNIFORM_LOCATION;
    shader->u_joint_matrices_loc = INVALID_GL_UNIFORM_LOCATION;
    shader->u_instanced_drawing_float_data_loc = INVALID_GL_UNIFORM_LOCATION;
    shader->u_instanced_drawing_uint_data_loc = INVALID_GL_UNIFORM_LOCATION;
    shader->uniform_locations = NULL;
    
    shader->wvp_mat_camera_index = INVALID_CAMERA_INDEX;

cleanup:
    return err;
}

static err_t create_gl_shader(uint32_t* out_gl_shader, GLenum shader_type, const char* shader_str) {
    err_t err = NO_ERROR;
    uint32_t gl_shader = INVALID_GL_SHADER;

    CHECK(out_gl_shader != NULL);
    *out_gl_shader = INVALID_GL_SHADER;

    CHECK_NO_GL_ERROR();

    RETHROW_IF_ERROR(gl_create_shader(&gl_shader, shader_type));

    RETHROW_IF_ERROR(gl_shader_source(gl_shader, shader_str));
    RETHROW_IF_ERROR(gl_compile_shader(gl_shader));

    CHECK_NO_GL_ERROR();

    *out_gl_shader = gl_shader;

cleanup:
    if (IS_ERROR(err) && gl_shader != INVALID_GL_SHADER) {
        gl_delete_shader(gl_shader);
        gl_shader = INVALID_GL_SHADER;
    }

    return err;
}

static void clean_shader(shader_t* shader) {
    if (shader != NULL) {
        if (shader->gl_program != INVALID_GL_PROGRAM) {
            if (shader->vert_gl_shader != INVALID_GL_SHADER) {
                glDetachShader(shader->gl_program, shader->vert_gl_shader);
            }
            
            if (shader->frag_gl_shader != INVALID_GL_SHADER) {
                glDetachShader(shader->gl_program, shader->frag_gl_shader);
            }

            glDeleteProgram(shader->gl_program);
            shader->gl_program = INVALID_GL_PROGRAM;
        }

        if (shader->vert_gl_shader != INVALID_GL_SHADER) {
            gl_delete_shader(shader->vert_gl_shader);
            shader->vert_gl_shader = INVALID_GL_SHADER;
        }
        
        if (shader->frag_gl_shader != INVALID_GL_SHADER) {
            gl_delete_shader(shader->frag_gl_shader);
            shader->frag_gl_shader = INVALID_GL_SHADER;
        }

        free(shader->attribute_locations);
        shader->attribute_locations = NULL;
        
        free(shader->uniform_locations);
        shader->uniform_locations = NULL;

        free(shader);
    }
}

err_t create_shader(shader_t** const out_shader,
                    const char* vert_shader_str, const char* frag_shader_str,
                    const char* attribute_names , uint32_t attributes_count,
                    const char* uniform_names, uint32_t uniforms_count) {
    err_t err = NO_ERROR;
    shader_t* shader = NULL;
    uint32_t shader_index = INVALID_SHADER_INDEX;
    uint32_t attribute_names_offset = 0;
    uint32_t uniform_names_offset = 0;

    CHECK(out_shader != NULL);
    *out_shader = NULL;

    CHECK(vert_shader_str != NULL);
    CHECK(frag_shader_str != NULL);
    CHECK(attribute_names != NULL);
    CHECK(uniform_names != NULL);

    CHECK(shaders_amount < MAX_SHADERS_AMOUNT);
    for (uint32_t i = 0; i < MAX_SHADERS_AMOUNT; i++) {
        if (shaders_list[i] == NULL) {
            shader_index = i;
            break;
        }
    }
    CHECK(IS_SHADER_INDEX_VALID(shader_index));

    shader = malloc(sizeof(shader_t));
    CHECK(shader != NULL);
    
    RETHROW_IF_ERROR(reset_shader(shader));

    CHECK_NO_GL_ERROR();

    RETHROW_IF_ERROR(create_gl_shader(&shader->vert_gl_shader, GL_VERTEX_SHADER, vert_shader_str));
    RETHROW_IF_ERROR(create_gl_shader(&shader->frag_gl_shader, GL_FRAGMENT_SHADER, frag_shader_str));

    RETHROW_IF_ERROR(gl_create_program(&shader->gl_program));
    RETHROW_IF_ERROR(gl_attach_shader(shader->gl_program, shader->vert_gl_shader));
    RETHROW_IF_ERROR(gl_attach_shader(shader->gl_program, shader->frag_gl_shader));

    RETHROW_IF_ERROR(gl_link_program(shader->gl_program));
    
    // shader user defined attributes
    shader->attribute_locations = malloc(sizeof(int32_t) * attributes_count);
    CHECK(shader->attribute_locations != NULL);
    attribute_names_offset = 0;
    for (uint32_t i = 0; i < attributes_count; i++) {
        RETHROW_IF_ERROR(gl_get_attribute_location(&shader->attribute_locations[i], shader->gl_program, &attribute_names[attribute_names_offset]));
        attribute_names_offset += strlen(&(attribute_names[attribute_names_offset])) + 1;
    }

    // shader default uniforms
    RETHROW_IF_ERROR(gl_get_uniform_location(&shader->u_texture_loc,                             shader->gl_program , "u_texture"));
    RETHROW_IF_ERROR(gl_get_uniform_location(&shader->u_camera_world_view_projection_matrix_loc, shader->gl_program , "u_camera_world_view_projection_matrix"));
    RETHROW_IF_ERROR(gl_get_uniform_location(&shader->u_instanced_drawing_float_data_loc,        shader->gl_program , "u_instanced_drawing_float_data"));
    RETHROW_IF_ERROR(gl_get_uniform_location(&shader->u_instanced_drawing_uint_data_loc,         shader->gl_program , "u_instanced_drawing_uint_data"));
    RETHROW_IF_ERROR(gl_get_uniform_location(&shader->u_joint_matrices_loc,                      shader->gl_program , "u_joint_matrices"));

    // shader user defined uniforms
    shader->uniform_locations = malloc(sizeof(int32_t) * uniforms_count);    
    CHECK(shader->uniform_locations != NULL);
    uniform_names_offset = 0;
    for (uint32_t i = 0; i < uniforms_count; i++) {
        RETHROW_IF_ERROR(gl_get_uniform_location(&shader->uniform_locations[i], shader->gl_program, &uniform_names[uniform_names_offset]));
        uniform_names_offset += strlen(&(uniform_names[uniform_names_offset])) + 1;
    }

    CHECK_NO_GL_ERROR();

    // add shader to shaders_list
    UNCONSTIFY(uint32_t, shader->shader_index) = shader_index;
    shaders_list[shader_index] = shader;
    shaders_amount += 1;
    
    shader->is_initialized = 1;

    *out_shader = shader;

cleanup:
    if (IS_ERROR(err) && shader != NULL) {
        clean_shader(shader);
        shader = NULL;
    }

    return err;
}

err_t create_shader_from_files( shader_t** const out_shader,
                                const char* vert_shader_file_path, const char* frag_shader_file_path,
                                const char* attribute_names , uint32_t attributes_count,
                                const char* uniform_names, uint32_t uniforms_count) {
    err_t err = NO_ERROR;
    char* vert_shader_str = NULL;
    char* frag_shader_str = NULL;

    CHECK(out_shader != NULL);
    *out_shader = NULL;

    CHECK(vert_shader_file_path != NULL);
    CHECK(frag_shader_file_path != NULL);
    CHECK(attribute_names != NULL);
    CHECK(uniform_names != NULL);
    
    load_file_contents(&vert_shader_str, vert_shader_file_path);
    CHECK(vert_shader_str != NULL);
    
    load_file_contents(&frag_shader_str, frag_shader_file_path);
    CHECK(frag_shader_str != NULL);

    RETHROW_IF_ERROR(create_shader(out_shader, vert_shader_str, frag_shader_str, attribute_names, attributes_count, uniform_names, uniforms_count));
    
cleanup:
    if (vert_shader_str != NULL) {
        free(vert_shader_str);
        vert_shader_str = NULL;
    }
    if (frag_shader_str != NULL) {
        free(frag_shader_str);
        frag_shader_str = NULL;
    }

    return err;
}

err_t update_shader_uniforms_by_camera(shader_t* shader, camera_t* camera) {
    err_t err = NO_ERROR;

    CHECK_SHADER(shader);
    CHECK_CAMERA(camera);

    CHECK(shader->u_camera_wvp_mat_loc != -1);

    // TODO: err_t wrap glUniformMatrix4fv
    DEBUG_CHECK_NO_GL_ERROR();
    glUniformMatrix4fv(shader->u_camera_wvp_mat_loc, 1, 0, camera->wvp_mat.mat);
    DEBUG_CHECK_NO_GL_ERROR();
    shader->wvp_mat_camera_index = camera->camera_index;

cleanup:
    return err;
}

err_t update_shader_uniforms_by_current_camera(shader_t* shader) {
    err_t err = NO_ERROR;
    camera_t* current_camera = NULL;

    CHECK_SHADER(shader);

    RETHROW_IF_ERROR(get_current_camera(&current_camera));

    if (current_camera == NULL) {
        goto cleanup;
    }

    RETHROW_IF_ERROR(update_shader_uniforms_by_camera(shader, current_camera));

cleanup:
    return err;
}

err_t update_current_shader_uniforms_by_camera(camera_t* camera) {
    err_t err = NO_ERROR;
    shader_t* current_shader = NULL;

    CHECK_CAMERA(camera);

    RETHROW_IF_ERROR(get_current_shader(&current_shader));

    if (current_shader == NULL) {
        goto cleanup;
    }

    RETHROW_IF_ERROR(update_shader_uniforms_by_camera(current_shader, camera));

cleanup:
    return err;
}

err_t use_shader(shader_t* shader) {
    err_t err = NO_ERROR;

    CHECK_SHADER(shader);

    if (shader->shader_index == current_shader) {
        goto cleanup;
    }

    RETHROW_IF_ERROR(gl_use_program(shader->gl_program));
    current_shader = shader->shader_index;
    
    RETHROW_IF_ERROR(update_shader_uniforms_by_current_camera(shader));

cleanup:
    return err;
}

err_t get_current_shader(shader_t** out_current_shader) {
    err_t err = NO_ERROR;

    CHECK(out_current_shader != NULL);
    *out_current_shader = NULL;
    
    CHECK(IS_SHADER_INDEX_VALID(current_shader));
    CHECK_SHADER(shaders_list[current_shader]);

    *out_current_shader = shaders_list[current_shader];

cleanup:
    return err;
}

void reset_all_shaders_wvp_mat_camera_index() {
    for (uint32_t i = 0; i < MAX_SHADERS_AMOUNT; i++) {
        if (shaders_list[i] == NULL) {
            continue;
        }

        if (!shaders_list[i]->is_initialized) {
            continue;
        }

        shaders_list[i]->wvp_mat_camera_index = INVALID_CAMERA_INDEX;
    }
}

err_t destroy_shader(shader_t* shader) {
    err_t err = NO_ERROR;
    
    CHECK_SHADER(shader);
    CHECK(current_shader != shader->shader_index);
    CHECK(shaders_amount > 0);
    
    shaders_list[shader->shader_index] = NULL;
    shaders_amount -= 1;
    
cleanup:
    clean_shader(shader);
    
    return err;
}

void clean_shaders() {
    DEBUG_PRINT("cleaning %u shaders\n", shaders_amount);
    for (uint32_t i = 0; i < MAX_SHADERS_AMOUNT; i++) {
        if (shaders_list[i] != NULL) {
            destroy_shader(shaders_list[i]);
        }
    }

    if (shaders_amount != 0) {
        DEBUG_PRINT("leaked a shader\n");
    }
    
    shaders_amount = 0;
}
