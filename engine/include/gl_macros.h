#ifndef __GL_MACROS_H__
#define __GL_MACROS_H__

#include "codeflow.h"
#include "glad/glad.h"

#define CHECK_NO_GL_ERROR()                                                 \
    do {                                                                    \
        GLenum gl_error = glGetError();                                     \
        err_t gl_error_as_error_value = gl_error;                           \
        if (gl_error_as_error_value == NO_ERROR) {                          \
            gl_error_as_error_value = GENERAL_ERROR;                        \
        }                                                                   \
        CHECK_WITH_ERROR(gl_error != GL_NO_ERROR, gl_error_as_error_value); \
    } while (0)

// glGetError() can be slow. For debug builds, check for each function. For release builds, use
// CHECK_NO_GL_ERROR once for a set of gl function calls, or in functions which are fine to be slow.
#ifdef DEBUG
#define DEBUG_CHECK_NO_GL_ERROR() CHECK_NO_GL_ERROR()
#else
#define DEBUG_CHECK_NO_GL_ERROR() do {} while (0)
#endif

#endif // __GL_MACROS_H__
