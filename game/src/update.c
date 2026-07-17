#include "game.h"

void game_camera_update() {
    // X axis rotation
    if (keys[SDL_SCANCODE_KP_2]) {
        game_camera->rx -= 0.05 * delta_frames;
    }
    if (keys[SDL_SCANCODE_KP_8]) {
        game_camera->rx += 0.05 * delta_frames;
    }

    // Z axis rotation
    if (keys[SDL_SCANCODE_KP_7]) {
        game_camera->rz -= 0.01 * delta_frames;
    }else if (keys[SDL_SCANCODE_KP_9]) {
        game_camera->rz += 0.01 * delta_frames;
    }else {
        game_camera->rz = 0;
    }

    // Y axis rotation
    if (keys[SDL_SCANCODE_KP_6]) {
        game_camera->ry -= 0.05 * delta_frames;
    }
    if (keys[SDL_SCANCODE_KP_4]) {
        game_camera->ry += 0.05 * delta_frames;
    }

    if (keys[SDL_SCANCODE_A]) {
        game_camera->x -= 1 * delta_frames;
    }
    if (keys[SDL_SCANCODE_D]) {
        game_camera->x += 1 * delta_frames;
    }
    if (keys[SDL_SCANCODE_S]) {
        game_camera->y -= 1 * delta_frames;
    }
    if (keys[SDL_SCANCODE_W]) {
        game_camera->y += 1 * delta_frames;
    }

    // perspective <-> orthographic togle
    if (keys[SDL_SCANCODE_P] == 1) {
        if (game_camera->is_prespective) {
            game_camera->is_prespective = 0;
            game_camera->near = -32000;
        } else {
            game_camera->is_prespective = 1;
            game_camera->near = 0.01;
        }
    }
}

err_t update() {
    game_camera_update();

    return NO_ERROR;
}
