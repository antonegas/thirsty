#version 460

in vec3 viewPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

layout (binding = 0) uniform sampler2D nes;

out vec4 outColor;

void main() {
  outColor = vec4(texture(nes, (vec2(viewPosition.x, -viewPosition.y) * 0.5 - vec2(-0.5, -0.5)) * 2.0).rgba);
  vec4 nesColor = texture(nes, vec2(fragTexCoord.x * 0.5, fragTexCoord.y * 0.5) + vec2(0.25, 0.25));
  nesColor = texture(nes, fragTexCoord);
  outColor = nesColor;
};