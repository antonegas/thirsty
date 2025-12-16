#version 460

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 originalPosition;
in vec3 viewPosition;

uniform vec4 fragColor;

out vec4 outColor;

void main() {
  const vec3 light_position = vec3(0.58, 0.58, 0.58);
  const vec3 light_position2 = vec3(-0.58, 0.58, 0.58);

  vec3 norm = normalize(fragNormal);
  vec3 light_direction = normalize(light_position - vec3(0.0, 0.0, 0.0));
  vec3 light_direction2 = normalize(light_position2 - vec3(0.0, 0.0, 0.0));
  float diffuse_scalar = max(dot(norm, light_direction), 0.0) * 2.5;
  float diffuse_scalar2 = max(dot(norm, light_direction2), 0.0) * 2.5;
  // diffuse_scalar = 1.0;

  outColor = diffuse_scalar * fragColor + diffuse_scalar2 * fragColor;
};