#ifndef __GAME_H__
#define __GAME_H__

#include "engine.h"
#include "cameras/cameras.h"
#include "frame_buffer_objects/frame_buffer_objects.h"

#define OUTPORT_WIDTH (380)
#define OUTPORT_HEIGHT (260)

#define OUTPORT_BACKGROUND_COLOR_R (71.0/255)
#define OUTPORT_BACKGROUND_COLOR_G (65.0/255)
#define OUTPORT_BACKGROUND_COLOR_B (107.0/255)

extern fbo_t* outport_fbo;

#endif // __GAME_H__
