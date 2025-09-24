#include "main.h"


const SDL_Event event;

const float TARGET_FRAME_DELAY;

uint8_t keys[SDL_NUM_SCANCODES]; // ticks since key pressed; 0 if released

uint8_t running;

const SDL_GLContext context;

SDL_Window* window;
int32_t window_width;
int32_t window_height;
int32_t window_drawable_width;
int32_t window_drawable_height;


// overload for `main`
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    UNUSED(hInstance);
    UNUSED(hPrevInstance);
    UNUSED(lpCmdLine);
    UNUSED(nCmdShow);

    return main(0, NULL);
}

int32_t main(int32_t argc, char** argv) {
    UNUSED(argc);
    UNUSED(argv);

    // #ifndef DEBUG
    // // hide console
    // FreeConsole();
    // #endif

    uint32_t init_result = engine_init();
    if (init_result != 0) exit(init_result);

    running = 1;
    while(running){
        while (SDL_PollEvent((SDL_Event*)&event)) engine_handle_event();
        
        engine_update();
        engine_render();
    }
    
    engine_clean();

    #ifdef DEBUG
    printf("ended sucessfully");
    #endif

    exit(0);
}

uint32_t backend_init() {
    // init sdl
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("Failed to initialize SDL\n");
        return 1;
    }

    // load default opengl dynamic library
    if (SDL_GL_LoadLibrary(NULL) != 0) {
        printf("Failed to dynamically load an OpenGL library\n");
        SDL_Quit();
        return 1;
    }

    // set context attributes - OpenGL 4.5 context (should be core)
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4) != 0) {
        printf("error\n");
        SDL_Quit();
        return 1;
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5) != 0) {
        printf("error\n");
        SDL_Quit();
        return 1;
    }
    if(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) != 0) {
        printf("error\n");
        SDL_Quit();
        return 1;
    }
    if(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) != 0) {
        printf("error\n");
        SDL_Quit();
        return 1;
    }

    // create window
    window = SDL_CreateWindow(
        "WINDOW_TITLE", // WINDOW_TITLE,
        0, // WINDOW_START_X,
        0, // WINDOW_START_Y,
        100, // WINDOW_START_WIDTH,
        100, // WINDOW_START_HEIGHT,
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
    );
    if (window == NULL) {
        SDL_Quit();
        printf("Failed to create window\n");
        return 1;
    }

    // init rendering context
    *((SDL_GLContext*)&context) = SDL_GL_CreateContext(window);
    if (context == NULL) {
        printf("Failed to create GL context\n");
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // make context current (should be current anyway)
    if (SDL_GL_MakeCurrent(window, context) != 0) {
        printf("Failed to make context current\n");
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // // use VSYNC
    // if (SDL_GL_SetSwapInterval(1) != 0) {
    //     printf("failed to enable Vsync\n");
    // }

    // retrieve GL functions
    if (gladLoadGLLoader(SDL_GL_GetProcAddress) == 0) {
        printf("Failed to retrieve GL functions\n");
        SDL_GL_DeleteContext(context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    #ifdef DEBUG
    // Check OpenGL properties
    printf("Vendor:   %s\n", glGetString(GL_VENDOR));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
    printf("Version:  %s\n", glGetString(GL_VERSION));
    #endif

    return 0;
}

uint32_t engine_init() {
    if (backend_init() != 0) {
        printf("Failed to init backend\n");
        return 1;
    }
    
    // get window sizes
    SDL_GetWindowSize(window, &window_width, &window_height);
    SDL_GL_GetDrawableSize(window, &window_drawable_width, &window_drawable_height);

    return 0;
}

void engine_handle_event() {
    SDL_Scancode scancode;

    switch(event.type) {
        case SDL_QUIT:
            running = 0;
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
    
    return;
}

void engine_update() {
    // keys - count ticks since press
    for (int32_t i = 0; i < SDL_NUM_SCANCODES; i++) {
        if (keys[i]) {
            keys[i]++;
            // avoid integer overflow
            if (keys[i] == 0) keys[i]--;
        }
    }
}

void engine_render() {
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

    // show drawn image - swap the buffers
    SDL_GL_SwapWindow(window);
    // wait until the buffers have been swaped
    glFinish();
}

void engine_clean() {
    SDL_GL_DeleteContext(context);
    #ifdef DEBUG
    printf("deleted context successfully\n");
    #endif
    SDL_DestroyWindow(window);
    #ifdef DEBUG
    printf("destroyed window successfully\n");
    #endif

    // TODO: on 'release', this function does not return!
    SDL_Quit();
}
