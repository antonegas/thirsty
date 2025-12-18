#include <SDL3/SDL.h>
#include <cmath>
#include <algorithm>

#include "Bottle.h"

#include "GL_utilities.h"
#include "LittleOBJLoader.h"
#include "VectorUtils4.h"
#include "LoadTGA.h"

GLuint Bottle::liquidShader = 0;
GLuint Bottle::glassShader = 0;
GLuint Bottle::lut = 0;
Model *Bottle::outside = nullptr;
Model *Bottle::inside = nullptr;
bool Bottle::initialized = false;

void Bottle::init() {
    // Load shaders
    liquidShader = loadShaders("shaders/bottle/liquid.vert", "shaders/bottle/liquid.frag");
    glassShader = loadShaders("shaders/bottle/glass.vert", "shaders/bottle/glass.frag");

    // Load textures
    LoadTGATextureSimple("models/bottle/lookup.tga", &lut);

    // Load models
    outside = LoadModel("models/bottle/outside.obj");
    inside = LoadModel("models/bottle/inside.obj");

    initialized = true;
}
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
    this->level = std::clamp(level, 0.1f, 0.9f);
}

void Bottle::setCubemap(GLuint cubemap) {
    this->cubemap = cubemap;
}

float Bottle::getLevel() {
    return level;
}

void Bottle::update(float delta) {
    constexpr float EPSILON = 0.035;

    if (abs(velocity.x) > EPSILON) {
        if (velocity.x < -EPSILON) {
            velocity.x += 0.04;
        } else {
            velocity.x -= 0.04;
        }

        foam = std::min(0.02, foam + 0.001);
    } else {
        if (abs(velocity.x) < 0.0001) {
            velocity.x = 0.0;
            foam = std::max(0.0, foam - 0.0001);
        } else {
            velocity.x *= 0.99;
        }
    }
}

void Bottle::render(float time, mat4 view, mat4 projection) {
    if (!initialized) {
        SDL_Log("Bottle class not initialized");
        return;
    }
    
    // Uniform uploads
    glUseProgram(liquidShader);
    glUniformMatrix4fv(glGetUniformLocation(liquidShader, "view"), 1, GL_TRUE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(liquidShader, "projection"), 1, GL_TRUE, projection.m);
    glUniformMatrix4fv(glGetUniformLocation(liquidShader, "rotation"), 1, GL_TRUE, rotation.m);
    glUniform1f(glGetUniformLocation(liquidShader, "velocity"), velocity.x);
    glUniform1f(glGetUniformLocation(liquidShader, "elapsedTime"), time);
    glUniform1f(glGetUniformLocation(liquidShader, "angle"), calculateAngle());
    glUniform1f(glGetUniformLocation(liquidShader, "percentage"), level);
    glUniform1f(glGetUniformLocation(liquidShader, "foam"), std::min(0.01f, foam));
    glUniform1f(glGetUniformLocation(liquidShader, "radius"), radius);
    glBindTextureUnit(0, lut);

    glUseProgram(glassShader);
    glUniformMatrix4fv(glGetUniformLocation(glassShader, "view"), 1, GL_TRUE, view.m);
    glUniformMatrix4fv(glGetUniformLocation(glassShader, "projection"), 1, GL_TRUE, projection.m);
    glUniformMatrix4fv(glGetUniformLocation(glassShader, "rotation"), 1, GL_TRUE, rotation.m);
    glUniform4fv(glGetUniformLocation(glassShader, "fragColor"), 1, glassColor.v);

    // Draw bottle
    // First draw opaque objects
    // Then transparent objects sorted back to front
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    // Draw back of liquid.
    glCullFace(GL_BACK);
    glUseProgram(liquidShader);
    glUniform4fv(glGetUniformLocation(liquidShader, "fragColor"), 1, liquidBackColor.v);
    DrawModel(inside, liquidShader, "vertPosition", "vertNormal", NULL);

    // Draw front of liquid.
    glCullFace(GL_FRONT);
    glUniform4fv(glGetUniformLocation(liquidShader, "fragColor"), 1, liquidFrontColor.v);
    DrawModel(inside, liquidShader, "vertPosition", "vertNormal", NULL);

    // Draw back of outside bottle.
    glCullFace(GL_BACK);
    glUseProgram(glassShader);
    DrawModel(outside, glassShader, "vertPosition", "vertNormal", NULL);

    // Draw back of inside bottle.
    DrawModel(inside, glassShader, "vertPosition", "vertNormal", NULL);

    // Draw front of inside bottle.
    glCullFace(GL_FRONT);
    DrawModel(inside, glassShader, "vertPosition", "vertNormal", NULL);

    // Draw front of outside bottle.
    DrawModel(outside, glassShader, "vertPosition", "vertNormal", NULL);

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