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

    frame = LoadModel("models/tv/tv.obj");
    screen = LoadDataToModel(
			(vec3 *)square, NULL, (vec2 *)squareTexCoord, NULL,
			squareIndices, 4, 6);

    initialized = true;
}

void Television::setScreen(GLuint texture) {
    screenTexture = texture;
}

void Television::update(float delta) {

}

void Television::render(float time, mat4 view, mat4 projection) {
    if (screenTexture == 0) return;

    glUseProgram(frameShader);
    glUniformMatrix4fv(glGetUniformLocation(frameShader, "view"), 1, GL_TRUE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(frameShader, "projection"), 1, GL_TRUE, projection.m);
    glUniformMatrix4fv(glGetUniformLocation(frameShader, "rotation"), 1, GL_TRUE, rotation.m);
    glUniformMatrix4fv(glGetUniformLocation(frameShader, "translation"), 1, GL_TRUE, translation.m);

    glUseProgram(screenShader);
    glUniformMatrix4fv(glGetUniformLocation(screenShader, "view"), 1, GL_TRUE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(screenShader, "projection"), 1, GL_TRUE, projection.m);
    glUniformMatrix4fv(glGetUniformLocation(screenShader, "rotation"), 1, GL_TRUE, rotation.m);
    glUniformMatrix4fv(glGetUniformLocation(screenShader, "translation"), 1, GL_TRUE, translation.m);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    DrawModel(frame, frameShader, "vertPosition", "vertNormal", "vertTexCoord");
    glBindTextureUnit(0, screenTexture);
    DrawModel(screen, screenShader, "vertPosition", "vertNormal", "vertTexCoord");
}