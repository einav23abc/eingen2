#ifndef __CAMERAS_H__
#define __CAMERAS_H__

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <limits.h>

#include "codeflow.h"
#include "../glad/glad.h"
#include "../vec_mat_quat/vec_mat_quat.h"


// fix wierd defines from <windows.h>
#ifdef near
#undef near
#endif
#ifdef far
#undef far
#endif


#ifndef M_PI
#define M_PI		3.14159265358979323846
#endif

#define wvp_mat world_view_projection_matrix
#define update_camera_wvp_mat update_camera_world_view_projection_matrix

#define INVALID_CAMERA_INDEX (UINT32_MAX)

#define CHECK_CAMERA(camera_ptr) RETHROW_IF_ERROR(check_camera(camera_ptr))


typedef struct camera_t {
    const uint32_t camera_index;

    uint8_t is_initialized : 1;

    uint8_t is_prespective : 1; // 0 -> orthographic , 1 -> prespective
    
    float x;
    float y;
    float z;
    
    float rx;
    float ry;
    float rz;

    float width;
    float height;
    float depth;

    float near;
    float far;
    float fov;

    float viewport_x;
    float viewport_y;
    float viewport_w;
    float viewport_h;

    mat4_t world_view_projection_matrix;
} camera_t;


err_t check_camera(camera_t* camera);

err_t create_camera(camera_t** const out_camera,
                    float x, float y, float z,
                    float rx, float ry, float rz,
                    float width, float height, float depth,
                    float near, float far,
                    uint8_t is_prespective, float fov,
                    float viewport_x, float viewport_y, float viewport_w, float viewport_h);

err_t use_camera(camera_t* camera);

err_t get_current_camera(camera_t** out_current_camera);

void set_no_current_camera();

err_t update_viewport_by_camera(camera_t* camera);

err_t update_viewport_by_current_camera();

void update_camera_world_view_projection_matrix(camera_t* camera);

void update_all_cameras_world_view_projection_matrix();

/* Destroys a camera.
 *
 * The camera will not be usable after destruction.
 *
 * Will not destroy the camera during render period (when `current_camera` is `-1`).
 * @param camera the camera to destroy.
*/
err_t destroy_camera(camera_t* camera);

/* Called by the engine when exiting.
 * Destroys all created cameras.
*/
void clean_cameras();

#endif // __CAMERAS_H__
