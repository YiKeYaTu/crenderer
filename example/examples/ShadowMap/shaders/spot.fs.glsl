#version 330 core

in vec3 vsWorldPosition;
in vec3 vsWorldNormal;
in vec2 vsTexCoord;

out vec4 FragColor;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform sampler2D uTexture;
uniform vec3 uLightPosition;

#define LIGHT_INTENSITY 1e4

#define KD 0.1f
#define KS 0.6f
#define KA 0.01f

vec3 renderUsingBlinnPhong(vec3 textureColor) {
    vec3 obj2light = uLightPosition - vsWorldPosition;

    float distance = length(obj2light);
    distance *= distance;

    vec3 diffuse = vec3(0), specular = vec3(0), ambient = vec3(0);

    diffuse += KD * textureColor * LIGHT_INTENSITY * max(0, dot(normalize(vsWorldNormal), normalize(obj2light))) / distance;
    ambient += KA * textureColor;

    return diffuse + specular + ambient;
}

void main() {
    vec4 textureColor = texture(uTexture, vsTexCoord);
    FragColor = vec4(renderUsingBlinnPhong(textureColor.xyz), 1.0);
//    FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}