#ifndef __GL_MACROS_H__
#define __GL_MACROS_H__

#include "codeflow.h"
#include "glad/glad.h"

#define FLUSH_LAST_GL_ERROR() ((void)glGetError())
#define CHECK_NO_GL_ERROR()                                                 \
    do {                                                                    \
        GLenum gl_error = glGetError();                                     \
        err_t gl_error_as_error_value = gl_error;                           \
        if (gl_error_as_error_value == NO_ERROR) {                          \
            gl_error_as_error_value = GENERAL_ERROR;                        \
        }                                                                   \
        CHECK_WITH_ERROR(gl_error != GL_NO_ERROR, gl_error_as_error_value); \
    } while (0)

#endif // __GL_MACROS_H__
