#version 460

uniform mat4 modelToWorld;
uniform mat4 worldToView;
uniform mat4 projection;

out vec3 fragNormal;
out vec2 fragTexCoord;

in vec3 vertPosition;
in vec3 vertNormal;
in vec2 vertTexCoord;

void main(void) {
  gl_Position = projection * worldToView * modelToWorld * vec4(vertPosition, 1.0);
  fragNormal = mat3(modelToWorld) * vertNormal;
  fragTexCoord = vertTexCoord;
}
