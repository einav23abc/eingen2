#include "cameras.h"

#include "macros.h"
#include "codeflow.h"

#include "../shaders/shaders.h"


#ifndef _MAX_CAMERAS_AMOUNT_
#define _MAX_CAMERAS_AMOUNT_ (128)
#endif

#define IS_CAMERA_INDEX_VALID(camera_index) (camera_index < MAX_CAMERAS_AMOUNT)


uint32_t cameras_amount = 0;
uint32_t current_camera = INVALID_CAMERA_INDEX;
camera_t* cameras_list[_MAX_CAMERAS_AMOUNT_];
const uint32_t MAX_CAMERAS_AMOUNT = _MAX_CAMERAS_AMOUNT_;


err_t check_camera(camera_t* camera) {
    err_t err = NO_ERROR;

    CHECK(camera != NULL);
    CHECK(IS_CAMERA_INDEX_VALID(camera->camera_index));
    CHECK(cameras_list[camera->camera_index] == camera);
    CHECK(camera->is_initialized);

cleanup:
    return err;
}

static void clean_camera(camera_t* camera) {
    free(camera);
    camera = NULL;
}

err_t create_camera(camera_t** const out_camera,
                    float x, float y, float z,
                    float rx, float ry, float rz,
                    float width, float height, float depth,
                    float near, float far,
                    uint8_t is_prespective, float fov,
                    float viewport_x, float viewport_y, float viewport_w, float viewport_h) {
    err_t err = NO_ERROR;
    camera_t* camera = NULL;
    uint32_t camera_index = INVALID_CAMERA_INDEX;

    CHECK(out_camera != NULL);
    *out_camera = NULL;

    CHECK(cameras_amount < MAX_CAMERAS_AMOUNT);
    for (uint32_t i = 0; i < MAX_CAMERAS_AMOUNT; i++) {
        if (cameras_list[i] == NULL) {
            camera_index = i;
            break;
        }
    }
    CHECK(IS_CAMERA_INDEX_VALID(camera_index));

    camera = malloc(sizeof(camera_t));
    CHECK(camera != NULL);

    camera->is_initialized = 0;

    camera->is_prespective = (is_prespective > 0);

    camera->x = x;
    camera->y = y;
    camera->z = z;

    camera->rx = rx;
    camera->ry = ry;
    camera->rz = rz;

    camera->width = width;
    camera->height = height;
    camera->depth = depth;

    camera->near = near;
    camera->far = far;
    camera->fov = fov;

    camera->viewport_x = viewport_x;
    camera->viewport_y = viewport_y;
    camera->viewport_w = viewport_w;
    camera->viewport_h = viewport_h;

    // TODO: RETHROW here
    update_camera_wvp_mat(camera);

    camera->is_initialized = 1;

    // add camera to cameras_list
    UNCONSTIFY(uint32_t, camera->camera_index) = camera_index;
    cameras_list[camera_index] = camera;
    cameras_amount += 1;

    *out_camera = camera;

cleanup:
    if (IS_ERROR(err) && camera != NULL) {
        clean_camera(camera);
        camera = NULL;
    }

    return err;
}

err_t use_camera(camera_t* camera) {
    err_t err = NO_ERROR;

    CHECK_CAMERA(camera);
    
    current_camera = camera->camera_index;
    RETHROW_IF_ERROR(update_viewport_by_camera(camera));

    RETHROW_IF_ERROR(update_current_shader_uniforms_by_camera(camera));

cleanup:
    return err;
}

err_t get_current_camera(camera_t** out_current_camera) {
    err_t err = NO_ERROR;

    CHECK(out_current_camera != NULL);
    *out_current_camera = NULL;
    
    if (current_camera == INVALID_CAMERA_INDEX) {
        goto cleanup;
    }

    CHECK(IS_CAMERA_INDEX_VALID(current_camera));
    CHECK_CAMERA(cameras_list[current_camera]);

    *out_current_camera = cameras_list[current_camera];

cleanup:
    return err;
}

err_t update_viewport_by_camera(camera_t* camera) {
    err_t err = NO_ERROR;

    CHECK_CAMERA(camera);
    
    glViewport(
        camera->viewport_x,
        camera->viewport_y,
        camera->viewport_w,
        camera->viewport_h
    );
    
cleanup:
    return err;
}

err_t update_viewport_by_current_camera() {
    err_t err = NO_ERROR;
    camera_t* current_camera = NULL;

    RETHROW_IF_ERROR(get_current_camera(&current_camera));
    if (current_camera != NULL) {
        RETHROW_IF_ERROR(update_viewport_by_camera(current_camera));
    }

cleanup:
    return err;
}

void update_camera_world_view_projection_matrix(camera_t* camera) {
    #define cx camera->x
    #define cy camera->y
    #define cz camera->z
    #define crx camera->rx
    #define cry camera->ry
    #define crz camera->rz
    #define cn camera->near
    #define cf camera->far
    #define cw camera->width
    #define ch camera->height

    const mat4_t rotation_x_matrix = (mat4_t){
        .mat = {
            1, 0,         0,        0,
            0, cos(crx), -sin(crx), 0,
            0, sin(crx),  cos(crx), 0,
            0, 0,         0,        1
        }
    };
    const mat4_t rotation_y_matrix = (mat4_t){
        .mat = {
                cos(cry), 0, sin(cry), 0,
                0,        1, 0,        0,
            -sin(cry), 0, cos(cry), 0,
                0,        0, 0,        1
        }
    };
    const mat4_t rotation_z_matrix = (mat4_t){
        .mat = {
            cos(crz), -sin(crz), 0, 0,
            sin(crz),  cos(crz), 0, 0,
            0,         0,        1, 0,
            0,         0,        0, 1
        }
    };

    const mat4_t world_matrix = (mat4_t){
        .mat = {
            1, 0, 0, -cx,
            0, 1, 0, -cy,
            0, 0, 1, -cz,
            0, 0, 0, 1
        }
    };
    
    mat4_t projection_matrix;
    if (camera->is_prespective) {
        const float _s_ = 1/tan(camera->fov*M_PI*0.5/180);
        projection_matrix  = (mat4_t){
            .mat = {
                _s_*ch/(cw+ch), 0,              0,           0,
                0,              _s_*cw/(cw+ch), 0,           0,
                0,              0,              cf/(cf-cn), -cf*cn/(cf-cn),
                0,              0,              1,           0
            }
        };
    }else{
        projection_matrix  = (mat4_t){
            .mat = {
                2.0/cw, 0,      0,            0,
                0,      2.0/ch, 0,            0,
                0,      0,      2.0/(cf-cn), -(cf+cn)/(cf-cn),
                0,      0,      0,            1
            }
        };
    }
    
    // rotation_x_matrix
    // rotation_y_matrix
    // rotation_z_matrix
    
    // projection_matrix
    // world_matrix
    // view_matrix

    mat4_t view_matrix = mat4_mul(mat4_mul(rotation_z_matrix, rotation_x_matrix), rotation_y_matrix);

    camera->world_view_projection_matrix = mat4_mul(mat4_mul(projection_matrix, view_matrix), world_matrix);

    #undef cx
    #undef cy
    #undef cz
    #undef crx
    #undef cry
    #undef crz
    #undef cn
    #undef cf
    #undef cw
    #undef ch
    return;
}

err_t destroy_camera(camera_t* camera) {
    err_t err = NO_ERROR;

    CHECK_CAMERA(camera);
    // TODO: make this possible
    // can only destroy cameras during render period
    CHECK(current_camera == INVALID_CAMERA_INDEX);
    CHECK(current_camera != camera->camera_index);
    CHECK(cameras_amount > 0);

    // shader->wvp_mat_camera_index
    
    cameras_list[camera->camera_index] = NULL;
    cameras_amount -= 1;
    
cleanup:
    clean_camera(camera);

    return err;
}

void clean_cameras() {
    DEBUG_PRINT("cleaning %u cameras\n", cameras_amount);
    for (uint32_t i = 0; i < MAX_CAMERAS_AMOUNT; i++) {
        if (cameras_list[i] != NULL) {
            destroy_camera(cameras_list[i]);
        }
    }

    if (cameras_amount != 0) {
        DEBUG_PRINT("leaked a camera\n");
    }

    cameras_amount = 0;
}
