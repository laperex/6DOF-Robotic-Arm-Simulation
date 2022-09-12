#version 450 core

flat in int vertexid;
flat in int instanceid;

uniform int data;

void main() {
	gl_FragData[1] = vec4(1, float(data), float(instanceid), float(vertexid));
	gl_FragData[0] = vec4(0);
}