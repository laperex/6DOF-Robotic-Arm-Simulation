#version 450 core

uniform sampler2D u_texture;
in vec2 uv;

void main() {
	gl_FragData[0] = texture(u_texture, uv);
	// gl_FragData[0] = vec4(1);
}