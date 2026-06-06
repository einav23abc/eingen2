#include "textures.h"

#include "gl_macros.h"

#ifndef _MAX_TEXTURES_AMOUNT_
#define _MAX_TEXTURES_AMOUNT_ (128)
#endif

#define IS_TEXTURE_INDEX_VALID(texture_index) (texture_index < MAX_TEXTURES_AMOUNT)

uint32_t textures_amount = 0;
texture_t* textures_list[_MAX_TEXTURES_AMOUNT_];
const uint32_t MAX_TEXTURES_AMOUNT = _MAX_TEXTURES_AMOUNT_;


err_t gl_delete_texture(uint32_t gl_texture) {
    err_t err = NO_ERROR;

    DEBUG_CHECK_NO_GL_ERROR();
    glDeleteTextures(1, &gl_texture);
    CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

err_t gl_generate_texture_2d(uint32_t* out_gl_texture,
                             GLint internal_format, GLint format, GLenum type,
                             uint32_t width, uint32_t height,
                             const void* data,
                             texture_param_setter_callback_t param_setter_callback) {
    err_t err = NO_ERROR;
    uint32_t gl_texture = INVALID_GL_TEXTURE;

    CHECK(out_gl_texture != NULL);
    *out_gl_texture = INVALID_GL_TEXTURE;
    
    CHECK_NO_GL_ERROR();

    glGenTextures(1, &gl_texture);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    DEBUG_CHECK_NO_GL_ERROR();

    glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, type, data);
    DEBUG_CHECK_NO_GL_ERROR();
    
    // default texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    DEBUG_CHECK_NO_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    DEBUG_CHECK_NO_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    DEBUG_CHECK_NO_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    DEBUG_CHECK_NO_GL_ERROR();

    // user set texture parameters
    if (param_setter_callback != NULL) {
        RETHROW_IF_ERROR(param_setter_callback());
    }
    DEBUG_CHECK_NO_GL_ERROR();

    *out_gl_texture = gl_texture;
    gl_texture = INVALID_GL_TEXTURE;

cleanup:
    if (IS_ERROR(err) && gl_texture != INVALID_GL_TEXTURE) {
        gl_delete_texture(gl_texture);
        gl_texture = INVALID_GL_TEXTURE;
    }

    return err;
}

err_t check_texture(texture_t* texture) {
    err_t err = NO_ERROR;

    CHECK(texture != NULL);
    CHECK(IS_TEXTURE_INDEX_VALID(texture->texture_index));
    CHECK(textures_list[texture->texture_index] == texture);

cleanup:
    return err;
}

err_t load_texture_from_surface_ext_params(texture_t** out_texture, SDL_Surface* texture_load_surface, texture_param_setter_callback_t param_setter_callback) {
    err_t err = NO_ERROR;
    texture_t* texture = NULL;
    uint32_t texture_index = INVALID_TEXTURE_INDEX;

    CHECK(out_texture != NULL);
    *out_texture = NULL;
    CHECK(texture_load_surface != NULL);
    CHECK(textures_amount < MAX_TEXTURES_AMOUNT);

    for (uint32_t i = 0; i < MAX_TEXTURES_AMOUNT; i++) {
        if (textures_list[i] == NULL) {
            texture_index = i;
            break;
        }
    }
    
    texture = malloc(sizeof(texture_t));
    CHECK(texture != NULL);

    UNCONSTIFY(uint32_t, texture->texture_index) = INVALID_TEXTURE_INDEX;
    texture->gl_texture = INVALID_GL_TEXTURE;

    RETHROW_IF_ERROR(gl_generate_texture_2d(
        &texture->gl_texture,
        GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
        texture_load_surface->w, texture_load_surface->h,
        texture_load_surface->pixels,
        param_setter_callback
    ));
    
    UNCONSTIFY(uint32_t, texture->texture_index) = texture_index;
    textures_list[texture_index] = texture;
    textures_amount += 1;

    *out_texture = texture;
    texture = NULL;

cleanup:
    if (IS_ERROR(err) && texture != NULL) {
        if (texture->gl_texture != INVALID_GL_TEXTURE) {
            gl_delete_texture(texture->gl_texture);
            texture->gl_texture = INVALID_GL_TEXTURE;
        }

        free(texture);
        texture = NULL;
    }

    return err;
}

err_t load_texture_from_surface(texture_t** out_texture, SDL_Surface* texture_load_surface) {
    err_t err = NO_ERROR;
    
    RETHROW_IF_ERROR(load_texture_from_surface_ext_params(out_texture, texture_load_surface, NULL));

cleanup:
    return err;
}

err_t load_texture_ext_params(texture_t** out_texture, const char* file_path, texture_param_setter_callback_t param_setter_callback) {
    err_t err = NO_ERROR;
    SDL_Surface* texture_load_surface = NULL;
    
    CHECK(out_texture != NULL);
    *out_texture = NULL;
    
    texture_load_surface = IMG_Load(file_path);
    CHECK(texture_load_surface != NULL);

    RETHROW_IF_ERROR(load_texture_from_surface_ext_params(out_texture, texture_load_surface, param_setter_callback));
    
cleanup:
    SDL_FreeSurface(texture_load_surface);
    
    return err;
}

err_t load_texture(texture_t** out_texture, const char* file_path) {
    err_t err = NO_ERROR;
    
    RETHROW_IF_ERROR(load_texture_ext_params(out_texture, file_path, NULL));

cleanup:
    return err;
}

err_t bind_gl_texture_to_uniform(uint32_t gl_texture, int32_t uniform_location, uint8_t texture_num) {
    err_t err = NO_ERROR;

    DEBUG_CHECK_NO_GL_ERROR();
    
    glUniform1i(uniform_location, texture_num);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glActiveTexture(GL_TEXTURE0 + texture_num);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glBindTexture(GL_TEXTURE_2D, gl_texture);
    DEBUG_CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

err_t bind_texture_to_uniform(texture_t* texture, int32_t uniform_location, uint8_t texture_num) {
    err_t err = NO_ERROR;

    CHECK_TEXTURE(texture);
    RETHROW_IF_ERROR(bind_gl_texture_to_uniform(texture->gl_texture, uniform_location, texture_num));

cleanup:
    return err;
}

static void clean_texture(texture_t* texture) {
    if (texture != NULL) {
        if (texture->gl_texture != INVALID_GL_TEXTURE) {
            gl_delete_texture(texture->gl_texture);
            texture->gl_texture = INVALID_GL_TEXTURE;
        }

        free(texture);
        texture = NULL;
    }

}

err_t destroy_texture(texture_t* texture) {
    err_t err = NO_ERROR;

    CHECK_TEXTURE(texture);
    CHECK(textures_amount > 0);

    textures_list[texture->texture_index] = NULL;
    textures_amount -= 1;
    
cleanup:
    clean_texture(texture);
    
    return err;
}

void clean_textures() {
    DEBUG_PRINT("cleaning %u textures\n", textures_amount);
    
    for (uint64_t i = 0; i < textures_amount; i++) {
        destroy_texture(textures_list[i]);
    }

    if (textures_amount != 0) {
        DEBUG_PRINT("leaked a textures\n");
    }

    textures_amount = 0;
}

void save_surface_to_c_file(const char* image_file_path, const char* name, const char* c_file_path) {
    FILE* fp;
    fp = fopen(c_file_path, "w");
    if (fp == NULL) {
        #ifdef DEBUG_SOFT_MODE
        printf("failed to open file \"%s\" to save surface \"%s\" to.\n", c_file_path, name);
        #endif
        return;
    }
    
    SDL_Surface* surface;
    surface = IMG_Load(image_file_path);
    if (surface == NULL){
        printf("failed to load image '%s'. error: \"%s\"\n", image_file_path, SDL_GetError());
        fclose(fp);
        return;
    }

    printf("\nprinting surface for saving:\n");
    if (surface->format->palette != NULL) printf("ATTENTION! format->palette isnt NULL. Might not work when saved.\n");
    if (surface->format->next != NULL)    printf("ATTENTION! format has next.           Might not work when saved.\n");
    if (surface->userdata != NULL)        printf("ATTENTION! userdata isnt NULL.        Might not work when saved.\n");
    if (surface->list_blitmap != NULL)    printf("ATTENTION! list_blitmap isnt NULL.    Might not work when saved.\n");
    if (surface->map != NULL)             printf("ATTENTION! map isnt NULL.             Might not work when saved.\n");
    printf("\n");

    fprintf(fp,
        ""      "#include <SDL2/SDL.h>\n"
        ""      "#include <stdint.h>\n"
    );

    // surface_format
    fprintf(fp,
        ""      "static SDL_PixelFormat %s_surface_format = (SDL_PixelFormat){\n"
        "\t"        ".format = %u,\n"
        "\t"        ".palette = 0x0,\n"
        "\t"        ".BitsPerPixel = %hhu,\n"
        "\t"        ".BytesPerPixel = %hhu,\n"
        "\t"        ".padding = {%hhu, %hhu},\n"
        "\t"        ".Rmask = %u,\n"
        "\t"        ".Gmask = %u,\n"
        "\t"        ".Bmask = %u,\n"
        "\t"        ".Amask = %u,\n"
        "\t"        ".Rloss = %hhu,\n"
        "\t"        ".Gloss = %hhu,\n"
        "\t"        ".Bloss = %hhu,\n"
        "\t"        ".Aloss = %hhu,\n"
        "\t"        ".Rshift = %hhu,\n"
        "\t"        ".Gshift = %hhu,\n"
        "\t"        ".Bshift = %hhu,\n"
        "\t"        ".Ashift = %hhu,\n"
        "\t"        ".refcount = %d,\n"
        "\t"        ".next = 0x0\n"
        ""      "};\n"
        ,
        name,
        surface->format->format,
        surface->format->BitsPerPixel,
        surface->format->BytesPerPixel,
        surface->format->padding[0], surface->format->padding[1],
        surface->format->Rmask,
        surface->format->Gmask,
        surface->format->Bmask,
        surface->format->Amask,
        surface->format->Rloss,
        surface->format->Gloss,
        surface->format->Bloss,
        surface->format->Aloss,
        surface->format->Rshift,
        surface->format->Gshift,
        surface->format->Bshift,
        surface->format->Ashift,
        surface->format->refcount
    );

    // surface_pixels
    fprintf(fp,
        ""      "static uint8_t %s_surface_pixels[] = {"
        ,
        name
    );
    for (int64_t i = 0; i < surface->w*surface->h*surface->format->BytesPerPixel; i++) {
        if (i != 0) fprintf(fp,", ");
        if (i%20 == 0) fprintf(fp,"\n\t");
        fprintf(fp,"%hhu", ((uint8_t*)(surface->pixels))[i]);
    }
    fprintf(fp,"\n};\n");

    // surface
    fprintf(fp,
        ""      "static SDL_Surface %s_surface = {\n"
        "\t"        ".flags = %u,\n"
        "\t"        ".format = &%s_surface_format,\n"
        "\t"        ".w = %d,\n"
        "\t"        ".h = %d,\n"
        "\t"        ".pitch = %d,\n"
        "\t"        ".pixels = &%s_surface_pixels,\n"
        "\t"        ".userdata = 0x0,\n"
        "\t"        ".locked = %d,\n"
        "\t"        ".list_blitmap = 0x0,\n"
        "\t"        ".clip_rect = (SDL_Rect){\n"
        "\t\t"          ".x = %d,\n"
        "\t\t"          ".y = %d,\n"
        "\t\t"          ".w = %d,\n"
        "\t\t"          ".h = %d,\n"
        "\t"        "},\n"
        "\t"        ".map = 0x0,\n"
        "\t"        ".refcount = %d\n"
        ""      "};\n"
        ,
        name,
        surface->flags,
        name,
        surface->w,
        surface->h,
        surface->pitch,
        name,
        surface->locked,
        surface->clip_rect.x,
        surface->clip_rect.y,
        surface->clip_rect.w,
        surface->clip_rect.h,
        surface->refcount
    );

    // name
    fprintf(fp,
        ""      "SDL_Surface* %s = &%s_surface;\n"
        ,
        name,
        name
    );

    SDL_FreeSurface(surface);
    fclose(fp);
}
