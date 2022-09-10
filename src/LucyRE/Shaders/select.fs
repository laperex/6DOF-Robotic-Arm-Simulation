#version 450 core

flat in int vertexid;
flat in int instanceid;

uniform int entity;
// uniform vec4 REF;

void main() {
	gl_FragData[0] = vec4(1, 1, 1, 1);
	gl_FragData[1] = vec4(1, float(entity), float(instanceid), float(vertexid));
	// discard;
}