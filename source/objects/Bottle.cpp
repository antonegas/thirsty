#include <SDL3/SDL.h>

#include "Bottle.h"

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "VectorUtils4.h"

// GLuint Bottle::glassProgram = 0;
// GLuint Bottle::liquidProgram = 0;
GLuint Bottle::shaderProgram = 0;
Model *Bottle::outside = nullptr;
Model *Bottle::inside = nullptr;
bool Bottle::initialized = false;

void Bottle::init() {
    // Load shaders
    shaderProgram = loadShaders("shaders/bottle/shader.vert", "shaders/bottle/shader.frag");

    // Load models
    outside = LoadModel("models/bottle/outside.obj");
    inside = LoadModel("models/bottle/inside.obj");

    initialized = true;
}

// void Bottle::setPosition(vec3 position) {
//     this->position = position;
// }

// void Bottle::setRotation(vec3 rotation) {
//     this->rotation = rotation;
// }

// void Bottle::setVelocity(vec3 velocity) {
//     // TODO: Introduce wobble.
//     this->velocity = velocity;
// }

// void Bottle::setAngular(vec3 angular) {
//     // TODO: Introduce wobble.
//     this->angular = angular;
// }

void Bottle::setRotation(mat4 rotation) {
    this->rotation = rotation;
}

void Bottle::rotate(mat4 rotation) {
    this->rotation = rotation * this->rotation;
}

void Bottle::setLevel(float level) {
    this->level = level;
}

void Bottle::update(float delta) {
    // position = position + velocity * delta;
    // rotation = rotation + angular * delta;
    // normal = R(angular * delta) * normal;

    // TODO: decrease wobble.
}

void Bottle::render(float time, mat4 projection) {
    // TODO: refraction
    // TODO: use plane in liquid frag.
    // 1. Draw backface outside using glass frag.
    // 2. Draw backface inside using glass frag.
    // 3. Draw backface inside using liquid frag.
    // 4. Draw frontface inside using liquid frag.
    // 5. Draw frontface inside using glass frag.
    // 6. Draw frontface outside using glass frag.

    if (!initialized) {
        SDL_Log("Bottle class not initialized");
        return;
    }

    glUseProgram(shaderProgram);

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

    glEnable(GL_DEPTH_TEST);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, glassColor.v);
    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_TRUE);
    glUniform1i(glGetUniformLocation(shaderProgram, "isLiquid"), GL_FALSE);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);

    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_FALSE);
    DrawModel(outside, shaderProgram, "vertPosition", "vertNormal", NULL);

    glEnable(GL_DEPTH_TEST);
}

float Bottle::calculatePlanePoint() {
    // TODO:
    // Binary search:
    // 1. Get intersection points with projected plane and flat model.
    // 2. Calculate area procentage of area using shoelace formula

    return 0.0;
}