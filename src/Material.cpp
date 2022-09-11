#include "Material.h"

void Material::Bind(lgl::Shader* shader) {
	assert(shader != nullptr);

	shader->Bind();

	shader->SetUniformVec3("material.ambient", &ambient[0]);
	shader->SetUniformVec3("material.diffuse", &diffuse[0]);
	shader->SetUniformVec3("material.specular", &specular[0]);
	shader->SetUniformf("material.shininess", shininess);
}

