#version 450 core

in vec3 normal;
in vec3 frag_pos;
in vec3 view_pos;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

struct Light {
	vec4 color;
	vec4 pos;

	float ambient;
	float diffuse;
	float specular;
};

uniform Light light;
uniform Material material;

flat in int vertexid;
flat in int instanceid;
uniform int data;

void main() {
	gl_FragData[1] = vec4(1, float(data), float(instanceid), float(vertexid));

	vec3 light_dir;
	if (light.pos.w == 0.0)
		light_dir = normalize(light.pos.xyz);
	else
		light_dir = normalize(light.pos.xyz - frag_pos);

	float diffuse = max(dot(light_dir, normal), 0.0);

	vec3 result = material.ambient * light.ambient;
	result += diffuse * material.diffuse * light.diffuse;

	if (diffuse != 0.0) {
		vec3 view_dir = normalize(view_pos - frag_pos);

		vec3 halfway = normalize(view_dir + light_dir);
		result += pow(max(dot(normal, halfway), 0.0), material.shininess) * material.specular * light.specular;
	}

	gl_FragData[0] = light.color * vec4(result, 1);
}
