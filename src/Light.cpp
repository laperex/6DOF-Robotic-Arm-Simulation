#include "Light.h"

void Light::Bind(lgl::Shader* shader) {
	assert(shader != nullptr);

	shader->Bind();
	
	shader->SetUniformVec4("light.pos", &pos[0]);
	shader->SetUniformVec4("light.color", &color[0]);
	shader->SetUniformf("light.ambient", ambient);
	shader->SetUniformf("light.diffuse", diffuse);
	shader->SetUniformf("light.specular", specular);
}
