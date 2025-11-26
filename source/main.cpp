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
int const DEFAULT_WIDTH = 600;
int const DEFAULT_HEIGHT = 400;

bool initGL();
void handleResize(SDL_WindowEvent *event);
void render();

// Positioning
Bottle bottle{};
mat4 projection{};
mat4 view{};



SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    SDL_SetHint(SDL_HINT_MAIN_CALLBACK_RATE, "60");

    if (!SDL_Init(SDL_INIT_GAMEPAD)) {
        SDL_Log("Couldn't initialize gamepad API:, %s", SDL_GetError());
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

    // init();

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

// TODO: temp
unsigned int shaderProgram;
unsigned int VBO, VAO;

float vertices[] = {
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
};

FBOstruct* fbo;
Model *model;

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

    // if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
    //     SDL_Log("Failed to initialize GLAD");
    //     return false;
    // }

    glViewport(0, 0, DEFAULT_WIDTH, DEFAULT_HEIGHT);
    // glClearColor(0.9, 0.3, 0.4, 0.0);

    // Init projection
    projection = frustum(-0.7, 0.7, -0.5, 0.5, 1.0, 280.0);
    view = lookAtv(vec3{0.0, 0.0, 10.0}, vec3{0.0, 0.0, 0.0}, vec3(0.0, 1.0, 0.0));

    // TODO: TEMP

    // build and compile our shader program
    // ------------------------------------
    // vertex shader
    // unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    // glCompileShader(vertexShader);
    // // check for shader compile errors
    // int success;
    // char infoLog[512];
    // glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    // // if (!success)
    // // {
    // //     glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
    // //     std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    // // }
    // // fragment shader
    // unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    // glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    // glCompileShader(fragmentShader);
    // // check for shader compile errors
    // glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    // // if (!success)
    // // {
    // //     glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
    // //     std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    // // }
    // // link shaders
    // shaderProgram = glCreateProgram();
    // glAttachShader(shaderProgram, vertexShader);
    // glAttachShader(shaderProgram, fragmentShader);
    // glLinkProgram(shaderProgram);
    // // check for linking errors
    // glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    // // if (!success) {
    // //     glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
    // //     std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    // // }
    // glDeleteShader(vertexShader);
    // glDeleteShader(fragmentShader);

    shaderProgram = loadShaders("./shaders/example/shader.vert", "./shaders/example/shader.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    vec3 v1{-0.0f, -0.5f, 0.0f};
    vec3 v2{0.5f, -0.5f, 0.0f};
    vec3 v3{0.0f,  0.5f, 0.0f};

    // GLfloat vertices[] = {
    //     v1.x, -0.5f, 0.0f, // left  
    //      0.5f, -0.5f, 0.0f, // right 
    //      0.0f,  0.5f, 0.0f  // top   
    // }; 

    vec3 vertices[] = {
        v1, v2, v3  
    };

    GLuint indices[] {
        0, 1, 2,
    };

    model = LoadDataToModel(vertices, nullptr, nullptr, nullptr, indices, sizeof(vertices), sizeof(indices));
    model = LoadModel("models/bottle/outside.obj");

    fbo = initFBO(100, 100, 0);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    
    return true;
}

void handleResize(SDL_WindowEvent *event) {
    glViewport(0, 0, event->data1, event->data2);
}

int i = 0;

void render() {
    i++;
    // if (i % 2) useFBO(fbo, 0, 0);
    // else useFBO(0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // // Since the bottle needs refraction it should be drawn last.
    // // TODO: draw bottle
    // bottle.render(projection, view);

    // printError("hello");

    // render
        // ------
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClearColor(0.9, 0.3, 0.4, 0.0);
        glClear(GL_COLOR_BUFFER_BIT);

        DrawModel(model, shaderProgram, "aPos", "normal", NULL);

        // draw our first triangle
        // glUseProgram(shaderProgram);
        // glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it every time, but we'll do so to keep things a bit more organized
        // glDrawArrays(GL_TRIANGLES, 0, 3);

    // Output to screen.
    SDL_GL_SwapWindow(window);
}