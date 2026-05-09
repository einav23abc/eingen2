#ifndef __CODEFLOW_H__
#define __CODEFLOW_H__

#include <stdint.h>
#include "macros.h"

typedef uint32_t err_t;

#ifdef IS_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR (0)
#define GENERAL_ERROR (1)

#ifdef IS_ERROR
#undef IS_ERROR
#endif
#define IS_ERROR(err) (err != NO_ERROR)


#define DEFAULT_CHECK_GOTO_LABEL cleanup

#define THROW_NO_GOTO_WITH_ERROR(error_value)   \
    do {                                        \
        err = error_value;                      \
    } while (0)

#define THROW_NO_GOTO() THROW_NO_GOTO_WITH_ERROR(GENERAL_ERROR)

#define THROW_GOTO_WITH_ERROR(label, error_value)   \
    do {                                            \
        err = error_value;                          \
        goto label;                                 \
    } while (0)

#define THROW_ERROR(error_value) THROW_GOTO_WITH_ERROR(DEFAULT_CHECK_GOTO_LABEL, error_value)
#define THROW() THROW_ERROR(GENERAL_ERROR)

#define CHECK_NO_GOTO_WITH_ERROR(exp, error_value)                                                  \
    do {                                                                                            \
        if (!(exp)) {                                                                               \
            err_t local_error_value = error_value;                                                  \
            DEBUG_PRINT("CHECK failed at %s:%u with error %d\n", __FILE__, __LINE__, error_value);  \
            THROW_NO_GOTO_WITH_ERROR(local_error_value);                                            \
        }                                                                                           \
    } while (0)

#define CHECK_GOTO_WITH_ERROR(exp, label, error_value)                                              \
    do {                                                                                            \
        if (!(exp)) {                                                                               \
            err_t local_error_value = error_value;                                                  \
            DEBUG_PRINT("CHECK failed at %s:%u with error %d\n", __FILE__, __LINE__, error_value);  \
            THROW_GOTO_WITH_ERROR(label, local_error_value);                                        \
        }                                                                                           \
    } while (0)

#define CHECK_WITH_ERROR(exp, error_value) CHECK_GOTO_WITH_ERROR(exp, DEFAULT_CHECK_GOTO_LABEL, error_value)
#define CHECK(exp) CHECK_WITH_ERROR(exp, GENERAL_ERROR) 

#define RETHROW_NO_GOTO_IF_ERROR(err_exp)                                           \
    do {                                                                            \
        err_t local_error_value = err_exp;                                          \
        CHECK_NO_GOTO_WITH_ERROR(!IS_ERROR(local_error_value), local_error_value);  \
    } while (0)

#define RETHROW_GOTO_IF_ERROR(err_exp, label)                                           \
    do {                                                                                \
        err_t local_error_value = err_exp;                                              \
        CHECK_GOTO_WITH_ERROR(!IS_ERROR(local_error_value), label, local_error_value);  \
    } while (0)

#define RETHROW_IF_ERROR(err_exp) RETHROW_GOTO_IF_ERROR(err_exp, DEFAULT_CHECK_GOTO_LABEL)

#endif // __CODEFLOW_H__
