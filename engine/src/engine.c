#include "engine.h"

__attribute__((weak)) uint32_t init();
__attribute__((weak)) void update();
__attribute__((weak)) void render();
__attribute__((weak)) void handle_event(const SDL_Event event);
__attribute__((weak)) void clean();

static SDL_Event event;

static uint8_t keys[SDL_NUM_SCANCODES]; // ticks since key pressed; 0 if released

static uint8_t is_running;

static SDL_GLContext context;

static SDL_Window* window;
static int32_t window_width;
static int32_t window_height;
static int32_t window_drawable_width;
static int32_t window_drawable_height;

static void engine_clean() {
    if (clean != NULL) {
        clean();
    }
    
    SDL_GL_DeleteContext(context);
    DEBUG_PRINT("deleted context successfully\n");
    SDL_DestroyWindow(window);
    DEBUG_PRINT("destroyed window successfully\n");

    // TODO: on 'release', this function does not return!
    SDL_Quit();
}

static uint32_t backend_init() {
    // init sdl
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        DEBUG_PRINT("Failed to initialize SDL\n");
        return 1;
    }

    // load default opengl dynamic library
    if (SDL_GL_LoadLibrary(NULL) != 0) {
        DEBUG_PRINT("Failed to dynamically load an OpenGL library\n");
        SDL_Quit();
        return 1;
    }

    // set context attributes - OpenGL 4.5 context (should be core)
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3) != 0) {
        DEBUG_PRINT("error\n");
        SDL_Quit();
        return 1;
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3) != 0) {
        DEBUG_PRINT("error\n");
        SDL_Quit();
        return 1;
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
        DEBUG_PRINT("error\n");
        SDL_Quit();
        return 1;
    }
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
        DEBUG_PRINT("error\n");
        SDL_Quit();
        return 1;
    }

    // create window
    window = SDL_CreateWindow(
        "WINDOW_TITLE", // WINDOW_TITLE,
        100, // WINDOW_START_X,
        100, // WINDOW_START_Y,
        400, // WINDOW_START_WIDTH,
        300, // WINDOW_START_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (window == NULL) {
        SDL_Quit();
        DEBUG_PRINT("Failed to create window\n");
        return 1;
    }

    // init rendering context
    context = SDL_GL_CreateContext(window);
    if (context == NULL) {
        DEBUG_PRINT("Failed to create GL context\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // make context current (should be current anyway)
    if (SDL_GL_MakeCurrent(window, context) != 0) {
        DEBUG_PRINT("Failed to make context current\n");
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // // use VSYNC
    // if (SDL_GL_SetSwapInterval(1) != 0) {
    //     DEBUG_PRINT("failed to enable Vsync\n");
    // }

    // retrieve GL functions
    if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0) {
        DEBUG_PRINT("Failed to retrieve GL functions\n");
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Check OpenGL properties
    DEBUG_PRINT("Vendor:   %s\n", glGetString(GL_VENDOR));
    DEBUG_PRINT("Renderer: %s\n", glGetString(GL_RENDERER));
    DEBUG_PRINT("Version:  %s\n", glGetString(GL_VERSION));

    return 0;
}

static uint32_t engine_init() {
    uint32_t game_init_result = 0;
    
    if (backend_init() != 0) {
        DEBUG_PRINT("Failed to init backend\n");
        return 1;
    }
    
    // get window sizes
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_GL_GetDrawableSize(window, &window_drawable_width, &window_drawable_height);

    if (init != NULL) {
        game_init_result = init();
        if (game_init_result != 0) {
            DEBUG_PRINT("init() failed with %u\n", game_init_result);
            engine_clean();
            return game_init_result;
        }
    }

    return 0;
}

static void engine_handle_event() {
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
        handle_event(event);
    }
    return;
}

static void engine_update() {
    //
    if (update != NULL) {
        update();
    }

    // keys - count ticks since press
    for (int32_t i = 0; i < SDL_NUM_SCANCODES; i++) {
        if (keys[i]) {
            keys[i]++;
            // avoid integer overflow
            if (keys[i] == 0) keys[i]--;
        }
    }
}

static void engine_render() {
    // clear window
    // glClearColor(BACKGROUND_COLOR_R, BACKGROUND_COLOR_G, BACKGROUND_COLOR_B, 1.0);
    glClearColor(1.0, 0, 0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    if (render != NULL) {
        render();
    }

    // show drawn image - swap the buffers
    SDL_GL_SwapWindow(window);
    // wait until the buffers have been swaped
    glFinish();
}

int32_t main(int32_t argc, char** argv) {
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
    uint32_t init_result = engine_init();
    if (init_result != 0) exit(init_result);

    is_running = 1;
    while(is_running){
        while (SDL_PollEvent(&event)) {
            engine_handle_event();
        }
        
        engine_update();
        engine_render();
    }
    
    DEBUG_PRINT("engine_clean()\n");
    engine_clean();

    DEBUG_PRINT("ended sucessfully\n");
    exit(0);
}

// overload for `main`
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNUSED(hInstance);
    UNUSED(hPrevInstance);
    UNUSED(lpCmdLine);
    UNUSED(nCmdShow);

    return main(0, NULL);
}
