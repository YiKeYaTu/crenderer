#version 330 core

in vec2 vTexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    FragColor = vec4(texture(uTexture, vTexCoord).xyz, 1.0);
//    FragColor = vec4(vTexCoord.y, 0.0, 0.0, 0.0);
}