#version 330 core

in vec3 outNormal;
in float ig;

out vec4 FragColor;

void main() {
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    // FragColor = vec4(-normal.x, -normal.y, -normal.z, 1.0);
    // if (float(ig < 0.0) == 1.0) {
    //     discard;
    // }
    float d = 1.0 - float(ig < 0.0);
    FragColor = vec4(outNormal, 1.0);
};