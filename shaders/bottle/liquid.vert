#version 460

in vec3 vertPosition;
in vec3 vertNormal;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 rotation;
uniform mat4 translation;
uniform float angle;
uniform float percentage;
uniform float radius;
uniform float vertFoam;
// uniform mat4 offset;

out vec3 fragNormal;
out vec3 fragPosition;
out vec3 originalPosition;
out vec3 viewPosition;
out float y;

layout (binding = 0) uniform sampler2D lut;

#define M_PI 3.1415926535897932384626433832795

void main() {
    vec4 position = rotation * vec4(vertPosition, 1.0);
    vec4 projectedPosition = projection * view * translation * position;

    // Output
    fragNormal = mat3(rotation) * vertNormal;
    fragPosition = position.xyz;
    originalPosition = vertPosition.xyz;
    viewPosition = projectedPosition.xyz;

    float fixedAngle = angle / (2.0 * M_PI);
    float lutValue = texture(lut, vec2(percentage, fixedAngle)).r;
    y = (lutValue + vertFoam * 0.5 - 0.5) * radius * 2.0;

    // Positioning
    gl_Position = projectedPosition;
}