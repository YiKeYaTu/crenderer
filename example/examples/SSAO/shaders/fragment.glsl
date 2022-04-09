#version 330 core

in vec2 gTexCoord;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    FragColor = texture(uTexture, gTexCoord);
}