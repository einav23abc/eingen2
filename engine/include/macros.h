#ifndef __MACROS_H__
#define __MACROS_H__

#define UNUSED(x) (void)(x)

#ifdef DEBUG
#define DEBUG_PRINT(...)        \
    do {                        \
        printf(__VA_ARGS__);    \
    } while (0)
#else
#define DEBUG_PRINT(...)    \
    do {                    \
    } while (0)
#endif

#define UNCONSTIFY(type, x) (*(type*)(&x))

#endif /* __MACROS_H__ */
