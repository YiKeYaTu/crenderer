#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 vNormal;
    vec2 vTexCoord;
} gs_in[];

out vec2 gTexCoord;

uniform float time;
float magnitude = 1.0;

vec4 calcFinalPosition(vec4 position, vec3 normal) {
//    return position + vec4(normal, 0.0) * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position;
}

void main() {
    gl_Position = calcFinalPosition(gl_in[0].gl_Position, gs_in[0].vNormal);
    gTexCoord = gs_in[0].vTexCoord;
    EmitVertex();

    gl_Position = calcFinalPosition(gl_in[1].gl_Position, gs_in[0].vNormal);
    gTexCoord = gs_in[1].vTexCoord;
    EmitVertex();

    gl_Position = calcFinalPosition(gl_in[2].gl_Position, gs_in[0].vNormal);
    gTexCoord = gs_in[2].vTexCoord;
    EmitVertex();

    EndPrimitive();
}