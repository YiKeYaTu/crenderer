#version 330 core

uniform sampler2D uShadowMap;

in vec3 vsLightProjectionSpacePosition;

out vec4 FragColor;

#define BIAS 1e-04

void main() {
    vec3 shadowMapCoord = vsLightProjectionSpacePosition * 0.5 + 0.5;

    FragColor = vec4(0.6);

    if (shadowMapCoord.x >= 0.0 && shadowMapCoord.x <= 1.0 && shadowMapCoord.y >= 0.0 && shadowMapCoord.y <= 1.0) {
        if (texture(uShadowMap, shadowMapCoord.xy).x + BIAS < shadowMapCoord.z) {
            FragColor = vec4(0.0);
        }
    }
}