#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <iterator>
#include <utility>
#include <string>
#include <memory>
#include <array>
#include <glad/glad.h>

// Ingemar headers
#include "GL_utilities.h"
#include "MicroGlut.h"
#include "LittleOBJLoader.h" 
#include "LoadTGA.h" 
#include "VectorUtils4.h" 

// Own headers
#include "Bottle.h"

using std::uint64_t;

SDL_Window *window;
SDL_GLContext glContext;

uint64_t const WINDOW_CONFIG = (
    SDL_WINDOW_RESIZABLE |
    SDL_WINDOW_OPENGL |
    SDL_WINDOW_MOUSE_CAPTURE
);

/* Window state */
bool fullscreen = false;
int const DEFAULT_WIDTH = 600;
int const DEFAULT_HEIGHT = 400;

bool initGL();
void handleResize(SDL_WindowEvent *event);
void render();

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");

    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize gamepad API:, %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    window = SDL_CreateWindow("THIRSTY", DEFAULT_WIDTH, DEFAULT_HEIGHT, WINDOW_CONFIG);
    
    if (!window) {
        SDL_Log("Couldn't create window: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!initGL()) {
        return SDL_APP_FAILURE;
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;
    }

    if (event->type == SDL_EVENT_WINDOW_RESIZED) {
        handleResize((SDL_WindowEvent*) event);
    }

    if (event->type == SDL_EVENT_KEY_DOWN) {
        if (((SDL_KeyboardEvent*)event)->mod & SDL_KMOD_CTRL) {
            // CTRL + KEY
            switch (((SDL_KeyboardEvent*)event)->key) {
                default:
                    break;
            }
        } else {
            switch (((SDL_KeyboardEvent*)event)->key) {
                case SDLK_F10:
                    fullscreen = !fullscreen;
                    SDL_SetWindowFullscreen(window, fullscreen);
                default:
                    break;
            }
        }
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    render();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}

bool initGL() {
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    glContext = SDL_GL_CreateContext(window);

    if (!glContext) {
        SDL_Log("Couldn't create OpenGL context", SDL_GetError());
        return false;
    }

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_Log("Failed to initialize GLAD");
        return false;
    }

    glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);

    Bottle::init();
    
    return true;
}

void handleResize(SDL_WindowEvent *event) {
    glViewport(0, 0, event->data1, event->data2);
}

void render() {

    // Since the bottle needs refraction it should be drawn last.
    // TODO: draw bottle

    // Output to screen.
    SDL_GL_SwapWindow(window);
}