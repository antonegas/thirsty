#version 460

in vec2 fragTexCoord;

layout (binding = 0) uniform sampler2D nes;

out vec4 outColor;

void main() {
  vec4 nesColor = texture(nes, fragTexCoord);
  outColor = nesColor;
};