#version 460

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 originalPosition;
in vec3 viewPosition;

uniform vec4 fragColor;
uniform float elapsedTime;
uniform float velocity;
uniform float angle;
uniform float percentage;
uniform float foam;
uniform float radius;

layout (binding = 0) uniform sampler2D lut;
layout (binding = 1) uniform sampler2D nes;

out vec4 outColor;

#define M_PI 3.1415926535897932384626433832795

void main() {
  const vec3 light_position = vec3(0.58, 0.58, 0.58);
  float level = fragPosition.y + sin(elapsedTime * 10 * velocity + fragPosition.x * 50) * 0.003 * velocity;

  float fixedAngle = angle / (2.0 * M_PI);
  float lutValue = texture(lut, vec2(percentage, fixedAngle)).r;
  float y = (lutValue + foam * 0.5 - 0.5) * radius * 2.0;
  float foamY = foam * radius * 2.0;

  if (level > y) {
    discard;
  }

  if (level > y - foam) {
    // Foam is hardcoded as white
    outColor = vec4(1.0, 1.0, 1.0, 1.0);
  } else {
    outColor = fragColor;
  }
};