#version 460

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 originalPosition;
in vec3 viewPosition;
in float y;

uniform vec4 fragColor;
uniform float elapsedTime;
uniform float velocity;
uniform float fragFoam;

layout (binding = 0) uniform sampler2D lut;

out vec4 outColor;

#define M_PI 3.1415926535897932384626433832795

void main() {
  const vec3 light_position = vec3(0.58, 0.58, 0.58);
  float level = fragPosition.y + sin(elapsedTime * 10 * velocity + fragPosition.x * 50) * 0.003 * velocity;

  if (level > y) {
    discard;
  }

  if (level > y - fragFoam) {
    // Foam is hardcoded as white
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    outColor = fragColor;
  }
};