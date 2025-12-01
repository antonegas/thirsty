#version 460

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 originalPosition;

uniform vec4 fragColor;
uniform bool isLiquid;

out vec4 outColor;

void main() {
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    // FragColor = vec4(-normal.x, -normal.y, -normal.z, 1.0);
    if (float(originalPosition.y > 0.0) == 1.0 && isLiquid) {
        discard;
    }
    // float d = 1.0 - float(ig < 0.0);
    outColor = fragColor;
};