#version 460

in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 rotation;
// uniform mat4 offset;

out vec3 fragNormal;
out vec2 fragTexCoord;

void main() {
    vec4 position = rotation * vec4(vertPosition, 1.0);
    vec4 projectedPosition = projection * view * position;

    // Output
    fragNormal = mat3(rotation) * vertNormal;
    fragTexCoord = vertTexCoord;

    // Positioning
    gl_Position = projectedPosition;

    // // Output
    // fragNormal = mat3(rotation) * vertNormal;
    // fragTexCoord = vertTexCoord;

    // // Positioning
    // gl_Position = position;
}