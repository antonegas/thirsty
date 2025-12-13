#version 460

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 originalPosition;
in vec3 viewPosition;

uniform vec4 fragColor;
uniform bool isLiquid;
uniform bool isInside;
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
  float level = originalPosition.y + sin(elapsedTime * 10 * velocity + originalPosition.x * 50) * 0.003 * velocity;

  float fixedAngle = angle / (2.0 * M_PI);
  float lutValue = texture(lut, vec2(percentage, fixedAngle)).r;
  float y = (lutValue - 0.5) * radius * 2.0;

  if (level > y && isLiquid) {
    discard;
  }

  vec3 norm = normalize(fragNormal);
  vec3 light_direction = normalize(light_position - vec3(0.0, 0.0, 0.0));
  float diffuse_scalar = max(dot(norm, light_direction), 0.0) * 2.5;

  outColor = (1.0 - float(isLiquid)) * diffuse_scalar * fragColor + float(isLiquid) * fragColor;
  if (isLiquid || true) {
    vec4 nesColor = vec4(texture(nes, vec2(viewPosition.x, -viewPosition.y) * 0.5 - vec2(-0.5, -0.5)).rgb, 1.0);
    vec4 whiteColor = vec4(1.0, 1.0, 1.0, 1.0);
    bool foam = level > y - 0.02;
    outColor = float(foam) * whiteColor + (1.0 - float(foam)) * nesColor;
    outColor = nesColor;
  }
};