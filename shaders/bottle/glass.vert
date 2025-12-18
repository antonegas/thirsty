#version 460

in vec3 vertPosition;
in vec3 vertNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 rotation;
uniform mat4 translation;
// uniform mat4 offset;

out vec3 fragNormal;
out vec3 fragPosition;
out vec3 originalPosition;
out vec3 viewPosition;

void main() {
    vec4 position = translation * rotation * vec4(vertPosition, 1.0);
    vec4 projectedPosition = projection * view * position;

    // Output
    fragNormal = mat3(rotation) * vertNormal;
    fragPosition = position.xyz;
    originalPosition = position.xyz;
    viewPosition = projectedPosition.xyz;

    // Positioning
    gl_Position = projectedPosition;
}