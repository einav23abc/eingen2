#ifndef __ENGINE_H__
#define __ENGINE_H__

#include <SDL2/SDL.h>
#include "glad/glad.h"
// #include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <windows.h>

#include "macros.h"
#include "codeflow.h"

int32_t get_window_width();
int32_t get_window_height();
int32_t get_window_drawable_width();
int32_t get_window_drawable_height();

#endif /* __ENGINE_H__ */
