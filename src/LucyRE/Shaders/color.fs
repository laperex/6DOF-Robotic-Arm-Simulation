#version 450 core

in vec4 color;

void main() {
	gl_FragData[0] = color;
}