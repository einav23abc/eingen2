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

    // follow the player
    game_camera->x = player_x      - 60*cos(game_camera->ry+M_PI*0.5)*cos(game_camera->rx);
    game_camera->y = player_y + 20 - 60*sin(game_camera->rx);
    game_camera->z = player_z      - 60*sin(game_camera->ry+M_PI*0.5)*cos(game_camera->rx);

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
    player_vx = 0;
    player_vz = 0;

    game_camera_update();

    // move left right (set velocity)
    if (keys[SDL_SCANCODE_A]) {
        player_vx += 2*cos(game_camera->ry+M_PI);
        player_vz += 2*sin(game_camera->ry+M_PI);
    }
    if (keys[SDL_SCANCODE_D]) {
        player_vx += 2*cos(game_camera->ry);
        player_vz += 2*sin(game_camera->ry);
    }

    // move in out (set velocity)
    if (keys[SDL_SCANCODE_S]) {
        player_vx += 2*cos(game_camera->ry+M_PI*1.5);
        player_vz += 2*sin(game_camera->ry+M_PI*1.5);
    }
    if (keys[SDL_SCANCODE_W]) {
        player_vx += 2*cos(game_camera->ry+M_PI*0.5);
        player_vz += 2*sin(game_camera->ry+M_PI*0.5);
    }

    player_x += player_vx * delta_frames;
    // player_y += player_vy * delta_frames;
    player_z += player_vz * delta_frames;

    // update ry appropriate to move direction
    if (player_vz != 0 || player_vx != 0) {
        while (player_ry >= M_PI*2) {player_ry -= M_PI*2;}
        while (player_ry < 0) {player_ry += M_PI*2;}

        float  goal_ry = atan(player_vz/player_vx)+M_PI*1.5;
        if (player_vx < 0) {
            goal_ry += M_PI;
        }

        while (goal_ry >= M_PI*2) {goal_ry -= M_PI*2;}
        while (goal_ry < 0) {goal_ry += M_PI*2;}
        
        // +-2PI for smallest diffrence
        if (fabs(player_ry-(goal_ry - M_PI*2)) < fabs(player_ry - goal_ry)) {
            goal_ry -= M_PI*2;
        }
        if (fabs(player_ry-(goal_ry + M_PI*2)) < fabs(player_ry - goal_ry)) {
            goal_ry += M_PI*2;
        }

        float delta_ry = 0.15 * delta_frames;
        player_ry = player_ry*(1.0 - delta_ry) + goal_ry * delta_ry;
    }

    if (player_current_animation != man_anim_run && (player_vx || player_vz)) {
        player_last_animation = player_current_animation;
        player_current_animation = man_anim_run;
        player_last_animation_frame = player_current_animation_frame;
        player_current_animation_frame = 0;
        player_animation_transition_frame = 0;
    }
    if (player_current_animation != man_anim_t_pose && (player_vx == 0 && player_vz == 0)) {
        player_last_animation = player_current_animation;
        player_current_animation = man_anim_t_pose;
        player_last_animation_frame = player_current_animation_frame;
        player_current_animation_frame = 0;
        player_animation_transition_frame = 0;
    }

    // player_last_animation_frame += delta_frames;
    player_current_animation_frame += delta_frames;
    player_animation_transition_frame += delta_frames;

    // DEBUG_PRINT("game_camera rotation: %f %f %f\n", game_camera->rx, game_camera->ry, game_camera->rz);
    // DEBUG_PRINT("player position: %f %f %f\n", player_x, player_y, player_z);

    return NO_ERROR;
}
