#version 460

in vec3 vertPosition;
in vec2 vertTexCoord;

out vec3 viewPosition;
out vec2 fragTexCoord;

void main() {
    viewPosition = vertPosition;
    fragTexCoord = vertTexCoord;
    gl_Position = vec4(vertPosition, 1.0);
}