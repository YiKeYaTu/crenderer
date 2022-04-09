#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

out VS_OUT {
    vec3 vNormal;
    vec2 vTexCoord;
} vs_out;

void main() {
    vs_out.vTexCoord = aTexCoord;
    vs_out.vNormal = normalize(aNormal);
    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}