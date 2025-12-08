#include <SDL3/SDL.h>
#include <cmath>
#include <algorithm>

#include "Bottle.h"

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "VectorUtils4.h"
#include "LoadTGA.h"

GLuint Bottle::shaderProgram = 0;
GLuint Bottle::lut = 0;
Model *Bottle::outside = nullptr;
Model *Bottle::inside = nullptr;
bool Bottle::initialized = false;

void Bottle::init() {
    // Load shaders
    shaderProgram = loadShaders("shaders/bottle/shader.vert", "shaders/bottle/shader.frag");

    // Load textures
    LoadTGATextureSimple("models/bottle/lookup.tga", &lut);

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

void Bottle::setVelocity(vec3 velocity) {
    this->velocity = velocity;
    
    if (this->velocity.x > 2.0) {
        this->velocity.x = 2.0;
    } else if (this->velocity.x < -2.0) {
        this->velocity.x = -2.0;
    }
}

void Bottle::setLevel(float level) {
    this->level = std::clamp(level, 0.2f, 0.8f);
}

float Bottle::getLevel() {
    return level;
}

void Bottle::update(float delta) {
    // position = position + velocity * delta;
    // rotation = rotation + angular * delta;
    // normal = R(angular * delta) * normal;

    // TODO: decrease wobble.

    if (velocity.x < -0.00001) {
        velocity.x += 0.07;
    } else if (velocity.x > 0.00001) {
        velocity.x -= 0.07;
    } else {
        velocity.x = 0.0;
    }
}

void Bottle::render(float time, mat4 projection) {
    // TODO: refraction
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
    
    // Uniform uploads
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_TRUE, projection.m);
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "rotation"), 1, GL_TRUE, rotation.m);
    glUniform1f(glGetUniformLocation(shaderProgram, "velocity"), velocity.x);
    glUniform1f(glGetUniformLocation(shaderProgram, "elapsedTime"), time);
    glUniform1f(glGetUniformLocation(shaderProgram, "angle"), calculateAngle());
    glUniform1f(glGetUniformLocation(shaderProgram, "percentage"), level);
    glUniform1f(glGetUniformLocation(shaderProgram, "radius"), radius);
    glBindTextureUnit(0, lut);

    glEnable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);

    // Draw back of outside bottle.
    glCullFace(GL_BACK);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, glassColor.v);
    glUniform1i(glGetUniformLocation(shaderProgram, "isLiquid"), GL_FALSE);
    DrawModel(outside, shaderProgram, "vertPosition", "vertNormal", NULL);

    // Draw back of liquid.
    glCullFace(GL_BACK);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, liquidBackColor.v);
    glUniform1i(glGetUniformLocation(shaderProgram, "isLiquid"), GL_TRUE);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);

    // Draw front of liquid.
    glCullFace(GL_FRONT);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, liquidFrontColor.v);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);

    // Draw front of inside bottle.
    glEnable(GL_DEPTH_TEST);
    glUniform4fv(glGetUniformLocation(shaderProgram, "fragColor"), 1, glassColor.v);
    glUniform1i(glGetUniformLocation(shaderProgram, "isLiquid"), GL_FALSE);
    DrawModel(inside, shaderProgram, "vertPosition", "vertNormal", NULL);

    // Draw front of outside bottle.
    glUniform1i(glGetUniformLocation(shaderProgram, "isInside"), GL_FALSE);
    DrawModel(outside, shaderProgram, "vertPosition", "vertNormal", NULL);

    glEnable(GL_DEPTH_TEST);
}

float Bottle::calculateAngle() {
    vec4 up{0.0, 1.0, 0.0, 1.0};
    vec4 origo{0.0, 0.0, 0.0, 1.0};
    vec4 other{0.0, 1.0, 0.0, 1.0};

    // TODO: Support translation
    vec4 direction = rotation * other - rotation * origo;

    float angle = acos(dot(up, direction));

    if (cross(up, direction).z > 0.0) {
        return M_PI - angle;
    } else {
        return M_PI + angle;
    }
}