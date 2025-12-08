#include <cmath>

#include "GamepadRotation.h"

void GamepadRotation::update(float delta) {
    rotation.x += gyroscope.x * delta;
    rotation.y += gyroscope.y * delta;
    rotation.z += gyroscope.z * delta;

    float magnitude = sqrt(
        accelerometer.x * accelerometer.x + 
        accelerometer.y * accelerometer.y + 
        accelerometer.z * accelerometer.z
    );

    if (magnitude < 2.0 * SDL_STANDARD_GRAVITY) {
        // Source: https://web.archive.org/web/20220518025559/https://www.pieter-jan.com/node/11
        // Turning around the X axis results in a vector on the Y-axis
        float xAcceleration = atan2(accelerometer.z, accelerometer.y) * 180 / M_PI;
        rotation.x = rotation.x * 0.98 + xAcceleration * 0.02;
 
	    // Turning around the Y axis results in a vector on the X-axis
        float zAcceleration = atan2(accelerometer.x, accelerometer.y) * 180 / M_PI;
        rotation.z = rotation.z * 0.98 + zAcceleration * 0.02;
    }
}

mat4 GamepadRotation::getMatrix() {
    return Ry(rotation.y) * Rx(rotation.x) * Rz(rotation.z);
}