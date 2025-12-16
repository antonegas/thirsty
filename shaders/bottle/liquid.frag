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
uniform float radius;

layout (binding = 0) uniform sampler2D lut;
layout (binding = 1) uniform sampler2D nes;

out vec4 outColor;

#define M_PI 3.1415926535897932384626433832795

void main() {
  const vec3 light_position = vec3(0.58, 0.58, 0.58);
  float level = viewPosition.y + sin(elapsedTime * 10 * velocity + viewPosition.x * 50) * 0.003 * velocity;

  float fixedAngle = angle / (2.0 * M_PI);
  float lutValue = texture(lut, vec2(percentage, fixedAngle)).r;
  float y = (lutValue - 0.5) * radius * 2.0;

  if (level > y) {
    discard;
  }

  outColor = fragColor;
};