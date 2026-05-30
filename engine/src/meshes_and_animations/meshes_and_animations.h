#ifndef __MESHES_AND_ANIMATIONS_H__
#define __MESHES_AND_ANIMATIONS_H__

#include <SDL2/SDL.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>

#include "glad/glad.h"
#include "codeflow.h"
#include "vec_mat_quat/vec_mat_quat.h"


#define INVALID_MESH_INDEX (UINT32_MAX)
#define INVALID_ANIMATION_INDEX (UINT32_MAX)

#define CHECK_MESH(mesh_ptr) RETHROW_IF_ERROR(check_mesh(mesh_ptr))
#define CHECK_ANIMATION(animation_ptr) RETHROW_IF_ERROR(check_animation(animation_ptr))


struct vbo_data_t;
struct joint_t;
struct mesh_t;
struct key_frame_t;
struct animation_t;


typedef struct vbo_data_t {
    size_t data_array_size;
    void* data_array;
    int32_t size;
    GLenum type;
    int32_t stride;
    uint32_t divisor;
} vbo_data_t;

typedef struct joint_t {
    uint32_t index;
    char* name;
    uint32_t parent;
    mat4_t inverse_bind_transform_mat;

    quat_vec_vec_t local_transform_qvv; // set and used by the drawing functions
    mat4_t model_transform_mat; // set and used by the drawing functions
} joint_t;

typedef struct mesh_t {
    const uint32_t mesh_index;

    uint8_t is_static;
    uint8_t is_binded;
    uint8_t is_data_preserved;
    
    uint32_t gl_vao;
    uint32_t vbos_amount;
    uint32_t* gl_vbos;
    // only relevent when data is preserved
    vbo_data_t* vbos_data;
    uint32_t indices_count;
    uint32_t* indices_array;
    
    uint32_t joints_amount;
    joint_t* joints;
    float* pose_joint_transform_matrices;
} mesh_t;

typedef struct key_frame_t {
    mat4_t joint_local_transform;
    quat_vec_vec_t joint_local_transform_qvv;
    float time_stamp;
} key_frame_t;

typedef struct joint_key_frame_t {
    uint32_t key_frames_amount;
    key_frame_t* key_frames;
} joint_key_frame_t;

typedef struct animation_t {
    const uint32_t animation_index;

    uint8_t is_static;

    uint32_t joints_amount;
    joint_key_frame_t* joints_key_frames;
} animation_t;


err_t check_mesh(mesh_t* mesh);
err_t check_animation(animation_t* animation);

err_t generate_mesh(mesh_t** out_mesh,
                    vbo_data_t* vbo_datas_arr, uint32_t vbo_datas_arr_size,
                    uint32_t* indices_array, uint32_t indices_count,
                    uint8_t should_skip_binding, uint8_t should_preserve_data);

err_t bind_mesh(mesh_t* mesh);

err_t generate_2d_quad_mesh(mesh_t** out_mesh,
                            float min_x, float max_x, float min_y, float max_y,
                            float min_u, float max_u, float min_v, float max_v,
                            uint8_t unbinded);
err_t mesh_generate_ball(mesh_t** out_mesh, uint32_t divisions, uint8_t unbinded);

err_t mesh_from_wavefront_obj_ext(mesh_t** out_mesh, const char* obj_file_path, quat_vec_vec_t transform_qvv, uint8_t unbinded);
err_t mesh_from_wavefront_obj(mesh_t** out_mesh, const char* obj_file_path, uint8_t unbinded);

err_t mesh_from_collada_dae_ext(mesh_t** out_mesh, const char* dae_file_path, quat_vec_vec_t transform_qvv, uint8_t unbinded);
err_t mesh_from_collada_dae(mesh_t** out_mesh, const char* dae_file_path, uint8_t unbinded);

err_t animation_from_collada_dae_ext(animation_t** out_animation, const char* dae_file_path, joint_t* joints, uint32_t joints_amount, quat_vec_vec_t transform_qvv);
err_t animation_from_collada_dae(animation_t** out_animation, const char* dae_file_path, joint_t* joints, uint32_t joints_amount);

// mix: between 0 and 1; 0 = no effect; 1 = overides current animation
err_t pose_mesh_mix_from_animation(mesh_t* mesh, animation_t* anim, float time_stamp, float mix);
err_t pose_mesh_set_from_animation(mesh_t* mesh, animation_t* anim, float time_stamp);

err_t draw_mesh_instanced(mesh_t* mesh, uint32_t instance_count);
err_t draw_mesh(mesh_t* mesh);
err_t draw_mesh_posed_instanced(mesh_t* mesh, uint32_t instance_count);
err_t draw_mesh_posed(mesh_t* mesh);

/* Destroys a mesh. The mesh will not be usable after destruction
 * @param mesh the mesh to destroy.
*/
err_t destroy_mesh(mesh_t* mesh);

/* Destroys an animation. The animation will not be usable after destruction
 * @param anim the animation to destroy.
*/
err_t destroy_animation(animation_t* anim);

/* Called by the engine when exiting.
 * Destroys all created meshes.
*/
void clean_meshes();
/* Called by the engine when exiting.
 * Destroys all created animations.
*/
void clean_animations();


/* @brief Save a mesh_t into a .c file.
 *
 * Can be used to load the saved mesh without having to load from a model file.
 * 
 * Binded meshes will not be saved beacuse they will not work if saved.
 * This is beacuse the mesh must be binded at each runtime.
 * 
 * Saved meshes do not have an index beacuse they dont need to be deallocated.
 * Beacuse of this, Using `destroy_mesh` on a saved mesh will have undefined behavior.
 * 
 * @param mesh The mesh that will be saved.
 * 
 * @param name The name that will be given to the mesh. This will be `mesh_t* name`
 * 
 * @param file_path The path to the c file where the mesh will be saved.
 */
void save_mesh_to_c_file(mesh_t* mesh, const char* name, const char* file_path);

/* @brief Save a animation_t into a .c file.
 *
 * The animation can then be used without having to load from a file.
 * 
 * Saved animations do not have an index beacuse they dont need to be deallocated.
 * Beacuse of this, Using `destroy_animation` on a saved animation will have undefined behavior.
 * 
 * @param anim The animation that will be saved.
 * 
 * @param name The name that will be given to the animation. This will be `animation_t* name`.
 * 
 * @param file_path The path to the c file where the animation_t will be saved.
 */
void save_animation_to_c_file(animation_t* anim, const char* name, const char* file_path);

#endif // __MESHES_AND_ANIMATIONS_H__
