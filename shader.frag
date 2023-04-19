#version 330 core

out vec4 FragColor;

in vec3 ourColor;

uniform float offsetAgain;
uniform float offsetAgainAgain;

void main(){
	FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0);
}