#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;
uniform vec3 uColor;

void main() {
    gl_Position = uProjection * uView * uModel * vec4(aPos + normalize(aNormal) * 0.01, 1.0);
}