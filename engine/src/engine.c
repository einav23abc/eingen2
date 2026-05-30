#include "engine.h"

#include "gl_macros.h"
#include "shaders/shaders.h"
#include "default_shader/default_shader.h"

__attribute__((weak)) err_t init();
__attribute__((weak)) err_t update();
__attribute__((weak)) err_t render();
__attribute__((weak)) err_t handle_event(const SDL_Event event);
__attribute__((weak)) err_t clean();

static SDL_Event event = {0};

static uint8_t keys[SDL_NUM_SCANCODES] = {0}; // ticks since key pressed; 0 if released

static uint8_t is_running = 0;

static SDL_GLContext context = NULL;

static SDL_Window* window = NULL;
static int32_t window_width = 0;
static int32_t window_height = 0;
static int32_t window_drawable_width = 0;
static int32_t window_drawable_height = 0;

static shader_t* default_shader = NULL;


static err_t engine_clean() {
    err_t err = NO_ERROR;

    if (clean != NULL) {
        RETHROW_IF_ERROR(clean());
    }
    
    SDL_GL_DeleteContext(context);
    DEBUG_PRINT("deleted context successfully\n");
    SDL_DestroyWindow(window);
    DEBUG_PRINT("destroyed window successfully\n");

    // TODO: on 'release', this function does not return!
    SDL_Quit();

cleanup:
    return err;
}

static err_t backend_init() {
    err_t err = NO_ERROR;

    CHECK(window == NULL);
    CHECK(context == NULL);

    // init sdl
    CHECK(SDL_Init(SDL_INIT_EVERYTHING) == 0);

    // load default opengl dynamic library
    CHECK(SDL_GL_LoadLibrary(NULL) == 0);

    // set context attributes - OpenGL 4.5 context (should be core)
    CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) == 0);
    CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) == 0);
    CHECK(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0);
    CHECK(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0);

    // create window
    window = SDL_CreateWindow(
        "WINDOW_TITLE", // WINDOW_TITLE,
        100, // WINDOW_START_X,
        100, // WINDOW_START_Y,
        400, // WINDOW_START_WIDTH,
        300, // WINDOW_START_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    CHECK(window != NULL);

    // init rendering context
    context = SDL_GL_CreateContext(window);
    CHECK(context != NULL);

    // make context current (should be current anyway)
    CHECK(SDL_GL_MakeCurrent(window, context) == 0);

    // // use VSYNC
    // if (SDL_GL_SetSwapInterval(1) != 0) {
    //     DEBUG_PRINT("failed to enable Vsync\n");
    // }

    // retrieve GL functions
    CHECK(gladLoadGLLoader(SDL_GL_GetProcAddress) != 0);
    
    // Check OpenGL properties
    DEBUG_PRINT("OpenGL properties:\n");
    DEBUG_PRINT("Vendor:   %s\n", glGetString(GL_VENDOR));
    DEBUG_PRINT("Renderer: %s\n", glGetString(GL_RENDERER));
    DEBUG_PRINT("Version:  %s\n", glGetString(GL_VERSION));

cleanup:
    if (IS_ERROR(err)) {
        if (context != NULL) {
            SDL_GL_DeleteContext(context);
            context = NULL;
        }
        if (window != NULL) {
            SDL_DestroyWindow(window);
            window = NULL;
        }
        SDL_Quit();
    }

    return err;
}

static err_t engine_init() {
    err_t err = NO_ERROR;
    
    RETHROW_IF_ERROR(backend_init());
    
    // get window sizes
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_GL_GetDrawableSize(window, &window_drawable_width, &window_drawable_height);

    RETHROW_IF_ERROR(create_shader(
        &default_shader,
        default_shader_vertex_shader, default_shader_fragment_shader,
        "in_vertex_position\0in_vertex_texcoord", 2,
        "", 0
    ));
    
    if (init != NULL) {
        RETHROW_IF_ERROR(init());
    }

cleanup:
    return err;
}

static err_t engine_handle_event() {
    err_t err = NO_ERROR;
    SDL_Scancode scancode = SDL_SCANCODE_UNKNOWN;

    switch(event.type) {
        case SDL_QUIT:
            is_running = 0;
            break;
        
        case SDL_KEYDOWN:
            scancode = event.key.keysym.scancode;
            if (scancode < SDL_NUM_SCANCODES && keys[scancode] == 0) keys[scancode] = 1;
            break;

        case SDL_KEYUP:
            scancode = event.key.keysym.scancode;
            if (scancode < SDL_NUM_SCANCODES) keys[scancode] = 0;
            break;
        
        case SDL_WINDOWEVENT:
            switch(event.window.event) {
                case SDL_WINDOWEVENT_RESIZED:
                    SDL_GetWindowSize(window, &window_width, &window_height);
                    SDL_GL_GetDrawableSize(window, &window_drawable_width, &window_drawable_height);
                    break;
            }
            break;
    }
    
    if (handle_event != NULL) {
        RETHROW_IF_ERROR(handle_event(event));
    }

cleanup:
    return err;
}

static err_t engine_update() {
    err_t err = NO_ERROR;

    if (update != NULL) {
        RETHROW_IF_ERROR(update());
    }

    // keys - count ticks since press
    for (int32_t i = 0; i < SDL_NUM_SCANCODES; i++) {
        if (keys[i]) {
            keys[i]++;
            // avoid integer overflow
            if (keys[i] == 0) keys[i]--;
        }
    }

cleanup:
    return err;
}

static err_t engine_render() {
    err_t err = NO_ERROR;

    // clear window
    // glClearColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, 1.0);
    glClearColor(1.0, 0, 0, 1.0);
    DEBUG_CHECK_NO_GL_ERROR();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    DEBUG_CHECK_NO_GL_ERROR();

    glEnable(GL_DEPTH_TEST);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glDepthFunc(GL_LESS);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glDepthMask(GL_TRUE);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glEnable(GL_BLEND);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glEnable(GL_CULL_FACE);
    DEBUG_CHECK_NO_GL_ERROR();
    
    glCullFace(GL_BACK);

    CHECK_NO_GL_ERROR();

    if (render != NULL) {
        RETHROW_IF_ERROR(render());
    }
    CHECK_NO_GL_ERROR();

    // show drawn image - swap the buffers
    SDL_GL_SwapWindow(window);
    // wait until the buffers have been swaped
    glFinish();

    CHECK_NO_GL_ERROR();

cleanup:
    return err;
}

int32_t main(int32_t argc, char** argv) {
    err_t err = NO_ERROR;

    UNUSED(argc);
    UNUSED(argv);

    #ifndef DEBUG
    // hide console
    FreeConsole();
    #endif

    if (init == NULL) DEBUG_PRINT("init() does not exist\n");
    if (render == NULL) DEBUG_PRINT("render() does not exist\n");
    if (update == NULL) DEBUG_PRINT("update() does not exist\n");
    if (handle_event == NULL) DEBUG_PRINT("handle_event() does not exist\n");
    if (clean == NULL) DEBUG_PRINT("clean() does not exist\n");

    DEBUG_PRINT("initializing engine\n");
    RETHROW_IF_ERROR(engine_init());

    is_running = 1;
    while(is_running){
        while (SDL_PollEvent(&event)) {
            RETHROW_IF_ERROR(engine_handle_event());
        }
        
        RETHROW_IF_ERROR(engine_update());
        RETHROW_IF_ERROR(engine_render());
    }
    
    DEBUG_PRINT("engine_clean()\n");
    
cleanup:
    RETHROW_NO_GOTO_IF_ERROR(engine_clean());

    DEBUG_PRINT("exiting with err_val %u\n", err);
    return IS_ERROR(err);
}

// overload for `main`
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNUSED(hInstance);
    UNUSED(hPrevInstance);
    UNUSED(lpCmdLine);
    UNUSED(nCmdShow);

    return main(0, NULL);
}
