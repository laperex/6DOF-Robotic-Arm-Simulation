#version 450 core

const vec4 data[6] = {
    vec4( -1.0,  1.0,  0.0, 1.0 ),
    vec4( -1.0, -1.0,  0.0, 0.0 ),
    vec4(  1.0, -1.0,  1.0, 0.0 ),
    vec4( -1.0,  1.0,  0.0, 1.0 ),
    vec4(  1.0, -1.0,  1.0, 0.0 ),
	vec4(  1.0,  1.0,  1.0, 1.0 ),
};

uniform vec2 res_uv;

out vec2 uv;

void main() {
	vec4 pos = data[gl_VertexID];
    gl_Position = vec4(pos.xy, 0.0, 1.0);
	uv = pos.zw * res_uv;
}