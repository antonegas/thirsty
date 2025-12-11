#version 460

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 originalPosition;

uniform vec4 fragColor;
uniform bool isLiquid;
uniform bool isInside;
uniform float elapsedTime;
uniform float velocity;
uniform float angle;
uniform float percentage;
uniform float radius;

uniform sampler2D lut;

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
};