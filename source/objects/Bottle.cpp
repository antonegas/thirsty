#include <Bottle.h>

#include "GL_utilities.h"
#include "LittleOBJLoader.h"

void Bottle::init() {
    glassProgram = loadShaders("./shaders/position.vert", "./shaders/bottle/glass.frag");
    liquidProgram = loadShaders("./shaders/position.vert", "./shaders/bottle/liquid.frag");
    inside = LoadModel("./models/bottle/inside.obj");
    outside = LoadModel("./models/bottle/outside.obj");
}

void Bottle::setPosition(vec3 position) {
    this->position = position;
}

void Bottle::setRotation(vec3 rotation) {
    this->rotation = rotation;
}

void Bottle::setVelocity(vec3 velocity) {
    // TODO: Introduce wobble.
    this->velocity = velocity;
}

void Bottle::setAngular(vec3 angular) {
    // TODO: Introduce wobble.
    this->angular = angular;
}

void Bottle::setLevel(float level) {
    this->level = level;
}

void Bottle::update(float delta) {
    position = position + velocity * delta;
    rotation = rotation + angular * delta;
    normal = R(angular * delta) * normal;

    // TODO: decrease wobble.
}

void Bottle::render(mat4 projection) {
    // TODO: refraction
    // TODO: use plane in liquid frag.
    // 1. Draw backface outside using glass frag.
    // 2. Draw backface inside using glass frag.
    // 3. Draw backface inside using liquid frag.
    // 4. Draw frontface inside using liquid frag.
    // 5. Draw frontface inside using glass frag.
    // 6. Draw frontface outside using glass frag.

    mat4 modelToWorld = T(position) * R(rotation);

    glUseProgram(glassProgram);
    glUniformMatrix4fv(glGetUniformLocation(glassProgram, "mtw"), 1, GL_TRUE, modelToWorld.m);

    glUseProgram(liquidProgram);
    glUniformMatrix4fv(glGetUniformLocation(liquidProgram, "mtw"), 1, GL_TRUE, modelToWorld.m);

    // TODO: Set actual input variable names
    glEnable(GL_CULL_FACE);
    
    // 1-3
    glCullFace(GL_BACK);

    // 1-2
    DrawModel(outside, glassProgram, "", "", "");
    DrawModel(inside, glassProgram, "", "", "");

    // 3-4
    DrawModel(inside, liquidProgram, "", "", "");

    // 4-6
    glCullFace(GL_FRONT);
    DrawModel(inside, liquidProgram, "", "", "");

    // 5-6
    glUseProgram(glassProgram);
    DrawModel(inside, glassProgram, "", "", "");
    DrawModel(outside, glassProgram, "", "", "");
}

float Bottle::calculatePlanePoint() {
    // TODO:
    // Binary search:
    // 1. Get intersection points with projected plane and flat model.
    // 2. Calculate area procentage of area using shoelace formula
}