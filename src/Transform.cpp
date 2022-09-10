#include "Transform.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

glm::mat4 Transform::GetTranslationMatrix() {
	// return -glm::translate(glm::mat4(1.0f), glm::normalize(parent_entity->GetRotationQuat() * glm::normalize(translation - parent_entity->translation)) * glm::length(translation - parent_entity->translation));
	return glm::translate(glm::mat4(1.0f), translation);
}

glm::mat4 Transform::GetScaleMatrix() {
	return glm::scale(glm::mat4(1.0f), scale);
}

glm::quat Transform::GetRotationQuat() {
	return glm::quat(glm::radians(rotation));
}

glm::mat4 Transform::GetRotationMatrix() {
	return glm::toMat4(GetRotationQuat());
}

glm::mat4 Transform::GetModelMatrix() {
	return GetTranslationMatrix() * GetRotationMatrix() * GetScaleMatrix();
}

void Transform::Clear() {
	translation = glm::vec3(0, 0, 0);
	scale = glm::vec3(0, 0, 0);
	rotation = glm::vec3(0, 0, 0);
}
