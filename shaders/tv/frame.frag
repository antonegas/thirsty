#version 460

in vec3 fragNormal;

layout (binding = 0) uniform sampler2D nes;

out vec4 outColor;

void main() {
  const vec3 light_position = vec3(0.58, 0.58, 0.58);

  vec3 norm = normalize(fragNormal);
  vec3 light_direction = normalize(light_position - vec3(0.0, 0.0, 0.0));
  float diffuse_scalar = min(max(dot(norm, light_direction), 0.0), 1.0);
  vec3 diffuse = diffuse_scalar * vec3(0.17, 0.17, 0.17);

  outColor = vec4(diffuse, 1.0);
};