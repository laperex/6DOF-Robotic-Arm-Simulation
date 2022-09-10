#version 450 core

in vec3 uvw;

uniform sampler2DArray u_texture;

void main() {
	gl_FragData[0] = texture(u_texture, uvw);
}