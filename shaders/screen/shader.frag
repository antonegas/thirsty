#version 460

in vec3 viewPosition;
in vec2 fragTexCoord;

layout (binding = 0) uniform sampler2D nes;

out vec4 outColor;

void main() {
  outColor = vec4(texture(nes, vec2(viewPosition.x, -viewPosition.y) * 0.5 - vec2(-0.5, -0.5)).rgba);
  // outColor
  outColor = texture(nes, vec2(fragTexCoord.x, -fragTexCoord.y)).rgba;
};