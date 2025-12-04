#version 460

in vec3 vertPosition;
in vec3 vertNormal;

uniform mat4 projection;
uniform mat4 rotation;
// uniform mat4 offset;

out vec3 fragNormal;
out vec3 fragPosition;
out vec3 originalPosition;

float scale = 4;

mat4 t = mat4(
    vec4(scale, 0.0, 0.0, 0.0),
    vec4(0.0, scale, 0.0, 0.0),
    vec4(0.0, 0.0, 1.0, 0.0),
    vec4(0.0, 0.0, 0.0, 1.0)
);

void main() {
    // vec4 position = t * projection * offset * rotation * vec4(vertPosition, 1.0);
    vec4 position2 = t * projection * rotation * vec4(vertPosition, 1.0);

    // Output
    fragNormal = vertNormal;
    fragPosition = position2.xyz;
    originalPosition = vertPosition;

    // Positioning
    gl_Position = position2;
}