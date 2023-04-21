#version 330 core

out vec4 FragColor;

in vec3 ourColor;

uniform float redShift;

void main(){
	FragColor = vec4(ourColor.x, ourColor.y * redShift, ourColor.z * redShift, 1.0);
}