#ifndef __MAIN_H__
#define __MAIN_H__

#include <SDL2/SDL.h>
#include "glad/glad.h"
#include <SDL2/SDL_image.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <windows.h>

#include "macros.h"

uint32_t backend_init();
uint32_t engine_init();
void engine_handle_event();
void engine_update();
/* Gets called by the function `main` for every frame.
 *
 * Calls the `render` function (if exists) after:
 *  1. The frame buffer was set to default.
 *  2. The color buffer and depth buffer have been cleared.
 *  3. The shader was set to `default_shader`.
 *  4. Every active camera's wvp_mat was updated.
 *  5. Depth test set to `less`.
 *  6. Back faces culled and face culling enabled.
 */
void engine_render();
void engine_clean();


#endif /* __MAIN_H__ */