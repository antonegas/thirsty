#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 normal;

uniform mat4 projection;
uniform mat4 rotation;
uniform mat4 offset;

out vec3 outNormal;
out float ig;

float scale = 4;

mat4 t = mat4(
    vec4(scale, 0.0, 0.0, 0.0),
    vec4(0.0, scale, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
);

mat4 t2 = mat4(
    vec4(1.0, 0.0, 0.0, 0.0),
    vec4(0.0, 1.0, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(-0.125, 0.0, 0.0, 1.0)
);

void main() {
    // vec4 pos = vec4(aPos, 1.0);
    vec4 pos = t * projection * offset * rotation * vec4(aPos, 1.0);
    // outNormal = mat3(rotation) * normal;
    outNormal = normal;
    ig = pos.y;

    gl_Position = pos;
}