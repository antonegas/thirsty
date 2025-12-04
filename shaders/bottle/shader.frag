#version 460

in vec3 fragNormal;
in vec3 fragPosition;
in vec3 originalPosition;

uniform vec4 fragColor;
uniform bool isLiquid;
uniform bool isInside;
uniform float elapsedTime;
uniform float velocity;

out vec4 outColor;

void main() {
    // FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
    // FragColor = vec4(-normal.x, -normal.y, -normal.z, 1.0);
    // bool levelFlat = velocity == 0.0;
    float level = fragPosition.y + sin(elapsedTime * 10 * velocity + fragPosition.x * 50) * 0.01 * velocity;
    // float level1 = sin(elapsedTime * 20 + originalPosition.x * 100) * 0.005;
    // float level2 = sin(elapsedTime * 20 + originalPosition.x * 50) * 0.0005;
    // float level = originalPosition.y * 4 + level1 + level2;
    // float plane = fragPosition.y * 4;
    // float level1 = cos(fragPosition.x * sin(elapsedTime / 1.5) * 200.0) * sin(elapsedTime * 20.0) * 0.01;
    // float level2 = sin(originalPosition.x * 400) * sin(elapsedTime * 40) * 0.01;
    // float level = plane + level1; 

    if (level > 0.0 && isLiquid) {
      discard;
    }
    // float d = 1.0 - float(ig < 0.0);
    outColor = fragColor;
};