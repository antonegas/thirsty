#version 460

in vec3 viewPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

layout (binding = 0) uniform sampler2D nes;

out vec4 outColor;

void main() {
  outColor = vec4(0.0, 0.0, 0.0, 1.0);
};