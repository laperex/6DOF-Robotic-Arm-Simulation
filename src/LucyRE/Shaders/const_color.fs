#version 450 core

uniform vec4 u_color;

void main() {
	gl_FragData[0] = u_color;
}