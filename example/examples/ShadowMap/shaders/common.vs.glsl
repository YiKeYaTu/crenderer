#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

uniform mat4 uModel;
uniform mat4 uView;
uniform mat4 uProjection;

uniform mat4 uLightView;
uniform mat4 uLightProjection;

out vec3 vsWorldPosition;
out vec3 vsWorldNormal;
out vec2 vsTexCoord;
out vec3 vsLightProjectionSpacePosition;

void main() {
    vsTexCoord = aTexCoord;
    vsWorldPosition = vec3(uModel * vec4(aPos, 1.0)).xyz;
    vsWorldNormal = vec3(uModel * vec4(aNormal, 1.0)).xyz;
    vsLightProjectionSpacePosition = vec3(uLightProjection * uLightView * uModel * vec4(aPos, 1.0));

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1.0);
}