#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#define MAIN
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
// #include <glad/glad.h>

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
constexpr int DEFAULT_WIDTH = 800;
constexpr int DEFAULT_HEIGHT = 800;

bool initGL();
void handleWindowResize(SDL_WindowEvent *event);
void handleWindowMove(SDL_WindowEvent *event);
void handleMouseMotion(SDL_MouseMotionEvent *event);
void resize(int width, int height);
void updateSize(int width, int height);
void updateOffset();
void updateRotation(vec3 axis, float angle);
void render();

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "120");

    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize gamepad API:, %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_Init(SDL_INIT_EVENTS)) {
        SDL_Log("Couldn't initialize events: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

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
        handleWindowResize((SDL_WindowEvent*) event);
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

    if (event->type == SDL_EVENT_WINDOW_MOVED) {
        handleWindowMove((SDL_WindowEvent*)event);
    }

    if (event->type == SDL_EVENT_MOUSE_MOTION) {
        handleMouseMotion((SDL_MouseMotionEvent*)event);
    }

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
    render();

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}

// TODO: temp
unsigned int shaderProgram;

FBOstruct* fbo;
Model *outside;
Model *inside;

mat4 rotation = IdentityMatrix();
mat4 projection = IdentityMatrix();
mat4 offset = IdentityMatrix();

vec4 glassColor{0.0, 0.3, 0.0, 0.1};
vec4 liquidBackColor{0.65098039, 0.61568627, 0.56862745, 1.0};
vec4 liquidFrontColor{0.64313725, 0.49019607, 0.49019607, 1.0};

// Bottle bottle;

bool initGL() {
    glContext = SDL_GL_CreateContext(window);
    
    if (!glContext) {
        SDL_Log("Couldn't create OpenGL context", SDL_GetError());
        return false;
    }
    
    if (!SDL_GL_MakeCurrent(window, glContext)) {
        SDL_Log("Couldn't make context current: %s", SDL_GetError());
        return false;
    }

    GLenum err = glewInit();
    if (GLEW_OK != err) {
        /* Problem: glewInit failed, something is seriously wrong. */
        SDL_Log("Failed to init glew: %s", glewGetErrorString(err));
        return false;
    }

    int width = 0;
    int height = 0;
    
    if (!SDL_GetWindowSize(window, &width, &height)) {
        SDL_Log("Failed to get window size: %s", SDL_GetError());
        return false;
    }

    glClearColor(1.0, 1.0, 1.0, 1.0);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    shaderProgram = loadShaders("shaders/bottle/shader.vert", "shaders/bottle/shader.frag");

    outside = LoadModel("models/bottle/outside.obj");
    inside = LoadModel("models/bottle/inside.obj");

    resize(width, height);
    updateSize(width, height);
    updateOffset();
    updateRotation({1.0, 0.0, 0.0}, 0.0);
    
    return true;
}

void handleWindowResize(SDL_WindowEvent *event) {
    resize(event->data1, event->data2);
    updateSize(event->data1, event->data2);
}

void handleWindowMove(SDL_WindowEvent *event) {
    updateOffset();
}

void handleMouseMotion(SDL_MouseMotionEvent *event) {
    if (!(SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON_LEFT)) return;

    vec3 u{-event->yrel, -event->xrel, 0.0};

    updateRotation(u, Norm(u) * 0.008);
}

void resize(int width, int height) {
    glViewport(0, 0, width, height);
}

void updateSize(int width, int height) {
    float smallest = std::min(width, height);
    projection.m[0] = smallest / width;
    projection.m[5] = smallest / height;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_TRUE, projection.m);
}

void updateOffset() {
    int width;
    int height;
    int x;
    int y;

    SDL_DisplayID displayID = SDL_GetDisplayForWindow(window);
    
    if (!displayID) {
        SDL_Log("Failed to get display: %s", SDL_GetError());
        return;
    }
    
    const SDL_DisplayMode *displayMode = SDL_GetDesktopDisplayMode(displayID);

    if (!displayMode) {
        SDL_Log("Failed to get display mode: %s", SDL_GetError());
        return;
    }
    
    if (!SDL_GetWindowSize(window, &width, &height)) {
        SDL_Log("Failed to get window size: %s", SDL_GetError());
        return;
    }

    if (!SDL_GetWindowPosition(window, &x, &y)) {
        SDL_Log("Failed to get window position: %s", SDL_GetError());
        return;
    }

    float wOffset = ((x - displayMode->w / 2) / (float)width + 0.5) * 0.5;
    float hOffset = ((y - displayMode->h / 2) / (float)height + 0.5) * 0.5;

    offset = T(-wOffset, hOffset, 0.0);

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "offset"), 1, GL_TRUE, offset.m);
}

void updateRotation(vec3 axis, float angle) {
    rotation = ArbRotate(axis, angle) * rotation;

    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotation"), 1, GL_TRUE, rotation.m);
}

void render() {
    GLfloat time = SDL_GetTicksNS() / 10e9;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Since the bottle needs refraction it should be drawn last.
    // TODO: draw bottle
    // bottle.render(projection, view);

    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    glClear(GL_COLOR_BUFFER_BIT);

    glDisable(GL_DEPTH_TEST);
    
    glCullFace(GL_BACK);
    // glDisable(GL_BLEND);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotation"), 1, GL_TRUE, (rotation).m);
    glUniform1f(glGetUniformLocation(shaderProgram, "elapsedTime"), time);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, glassColor.v);
    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_FALSE);
    glUniform1i(glGetUniformLocation(shaderProgram, "isLiquid"), GL_FALSE);
    DrawModel(outside, shaderProgram, "vertPosition", "vertNormal", NULL);

    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_TRUE);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);
    
    // glDisable(GL_DEPTH_TEST);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, liquidBackColor.v);
    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_FALSE);
    glUniform1i(glGetUniformLocation(shaderProgram, "isLiquid"), GL_TRUE);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);

    glCullFace(GL_FRONT);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, liquidFrontColor.v);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);

    // glEnable(GL_DEPTH_TEST);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, glassColor.v);
    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_TRUE);
    glUniform1i(glGetUniformLocation(shaderProgram, "isLiquid"), GL_FALSE);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);

    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_FALSE);
    DrawModel(outside, shaderProgram, "vertPosition", "vertNormal", NULL);

    // Output to screen.
    SDL_GL_SwapWindow(window);
}