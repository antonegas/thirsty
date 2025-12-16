#include <SDL3/SDL.h>
#include <glew.h>

#include "Television.h"

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "VectorUtils4.h"
#include "LoadTGA.h"

GLuint Television::screenShader = 0;
GLuint Television::frameShader = 0;
GLuint Television::screenTexture = 0;
Model *Television::frame = nullptr;
Model *Television::screen = nullptr;
bool Television::initialized = false;

void Television::init() {
    // Load shaders
    frameShader = loadShaders("shaders/tv/position.vert", "shaders/tv/frame.frag");
    screenShader = loadShaders("shaders/tv/position.vert", "shaders/tv/screen.frag");

    // Load models
    // TODO: temp
    vec3 square[] = {
        vec3{-0.24, 0.19, 0.0},
        vec3{-0.24, -0.19, 0.0},
        vec3{0.24, -0.19, 0.0},
        vec3{0.24, 0.19, 0.0}
    };

    GLfloat squareTexCoord[] = {
        0.0, 0.0,
        0.0, 1.0,
        1.0, 1.0,
        1.0, 0.0
    };
    GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

//     GLfloat square[] = {
// 							-1,-1,0,
// 							-1,1, 0,
// 							1,1, 0,
// 							1,-1, 0};
// GLfloat squareTexCoord[] = {
// 							 0, 0,
// 							 0, 1,
// 							 1, 1,
// 							 1, 0};
// GLuint squareIndices[] = {0, 1, 2, 0, 2, 3};

    frame = LoadModel("models/tv/tv.obj");
    screen = LoadDataToModel(
			(vec3 *)square, NULL, (vec2 *)squareTexCoord, NULL,
			squareIndices, 4, 6);
    // screen = LoadDataToModel(square, NULL, (vec2 *)squareTexCoord, NULL, squareIndices, 4, 6);

    initialized = true;
}

void Television::setScreen(GLuint texture) {
    screenTexture = texture;
}

void Television::update(float delta) {

}

void Television::render(float time, mat4 projection) {
    if (screenTexture == 0) return;
    DrawModel(frame, frameShader, "vertPosition", "vertNormal", "vertTexCoord");
    glCullFace(GL_BACK);
    glBindTextureUnit(0, screenTexture);
    DrawModel(screen, screenShader, "vertPosition", NULL, "vertTexCoord");
    glCullFace(GL_FRONT);
}