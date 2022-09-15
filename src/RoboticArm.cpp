#include "RoboticArm.h"
#include <imgui_lucy_impl.h>
#include <LucyRE/LucyRE.h>
#include <iostream>
#include "Serializer.h"
#include "Camera.h"
#include "Transform.h"
#include "ECS.h"
#include "Window.h"
#include "Events.h"

static auto& registry = Registry::Instance();

static struct {
	glm::vec3 position = { 0, 0, 0 };
	UTIL_UUID mesh_id = UTIL_NULL_UUID;
	Transform transform;
	glm::mat4 matrix;
} lower_base = {{ 44.0448, 71.6270, 55.25 }},
upper_base = {{ 44.0448, 71.6270, 55.25 }},
lower_elbow = {{ 44.2948, 86.6948, 55.25 }},
upper_elbow = {{ 44.2948, 190.8709, 55.25 }},
wrist = {{ 43.2270, 285.2090, 55.25 }};

void RoboticArm::CalculateMatrices(Position position) {
	auto& window = registry.store<Window>();

	if (lower_base.mesh_id == UTIL_NULL_UUID) {
		lower_base.mesh_id = lre::InsertMesh("lower_base", util::LoadMeshToGPU("C:\\Users\\krishnadev\\Documents\\LOWER BASE.stl", "", -lower_base.position));
	}
	if (upper_base.mesh_id == UTIL_NULL_UUID) {
		upper_base.mesh_id = lre::InsertMesh("upper_base", util::LoadMeshToGPU("C:\\Users\\krishnadev\\Documents\\UPPER BASE.stl", "", -upper_base.position));
	}
	if (lower_elbow.mesh_id == UTIL_NULL_UUID) {
		lower_elbow.mesh_id = lre::InsertMesh("lower_elbow", util::LoadMeshToGPU("C:\\Users\\krishnadev\\Documents\\LOWER ELBOW.stl", "", -lower_elbow.position));

		lower_elbow.transform.translation = lower_elbow.position - lower_base.position;
	}
	if (upper_elbow.mesh_id == UTIL_NULL_UUID) {
		upper_elbow.mesh_id = lre::InsertMesh("upper_elbow", util::LoadMeshToGPU("C:\\Users\\krishnadev\\Documents\\UPPER ELBOW.stl", "", -upper_elbow.position));
	
		upper_elbow.transform.translation = upper_elbow.position - lower_base.position;
	}
	if (wrist.mesh_id == UTIL_NULL_UUID) {
		wrist.mesh_id = lre::InsertMesh("wrist", util::LoadMeshToGPU("C:\\Users\\krishnadev\\Documents\\ARM.stl", "", -wrist.position));

		wrist.transform.translation = wrist.position - lower_base.position;
	}

	// Set Rotation
	upper_base.transform.rotation = { 0, position.base - 90, 0 };
	lower_elbow.transform.rotation = { 0, 0, position.lower_elbow - 90 };
	upper_elbow.transform.rotation = { 0, 0, position.upper_elbow + position.lower_elbow + 90 };
	wrist.transform.rotation = { 0, 0, position.wrist + position.upper_elbow + position.lower_elbow - 90 };

	// Set Translation
	upper_elbow.transform.translation = lower_elbow.transform.translation + (((upper_elbow.position - lower_elbow.position)) * glm::quat(glm::radians(glm::vec3(0, 0, -lower_elbow.transform.rotation.z))));
	wrist.transform.translation = upper_elbow.transform.translation + (((wrist.position - upper_elbow.position)) * glm::quat(glm::radians(glm::vec3(0, 0, -upper_elbow.transform.rotation.z))));

	lower_base.matrix = lower_base.transform.GetTranslationMatrix();
	upper_base.matrix = upper_base.transform.GetRotationMatrix() * upper_base.transform.GetTranslationMatrix();
	lower_elbow.matrix = upper_base.transform.GetRotationMatrix() * lower_elbow.transform.GetTranslationMatrix() * lower_elbow.transform.GetRotationMatrix();
	upper_elbow.matrix = upper_base.transform.GetRotationMatrix() * upper_elbow.transform.GetTranslationMatrix() * upper_elbow.transform.GetRotationMatrix();
	wrist.matrix = upper_base.transform.GetRotationMatrix() * wrist.transform.GetTranslationMatrix() * wrist.transform.GetRotationMatrix();
}

void RoboticArm::Render() {
	auto* shader = lre::GetShader("phong");

	lre::SetModel(lower_base.matrix);
	lre::RenderMesh(lower_base.mesh_id, shader, 1);
	lre::SetModel(upper_base.matrix);
	lre::RenderMesh(upper_base.mesh_id, shader, 2);
	lre::SetModel(lower_elbow.matrix);
	lre::RenderMesh(lower_elbow.mesh_id, shader, 3);
	lre::SetModel(upper_elbow.matrix);
	lre::RenderMesh(upper_elbow.mesh_id, shader, 4);
	lre::SetModel(wrist.matrix);
	lre::RenderMesh(wrist.mesh_id, shader, 5);
}

bool RoboticArm::IsNamePresent(std::string name) {
	for (auto& pair: animation_registry) {
		if (pair.second.name == name) {
			return true;
		}
	}

	return false;
}

std::string RoboticArm::GetName(std::string name) {
	std::string result = name;

	for (int i = 1; IsNamePresent(result); i++) {
		result = name + " (" + std::to_string(i) + ")";
	}

	return result;
}

UTIL_UUID RoboticArm::NewAnimation(std::string name, Animation animation, UTIL_UUID id) {
	assert(animation_registry.find(id) == animation_registry.end());

	if (name == "")
		name = "New Animation";

	name = GetName(name);

	animation_registry[id] = { name, animation };

	return id;
}

Animation* RoboticArm::GetAnimation(UTIL_UUID id) {
	if (animation_registry.find(id) != animation_registry.end()) {
		return &animation_registry[id].animation;
	}

	return nullptr;
}

void RoboticArm::SetSelectedAnimation(Animation* animation) {
	selected_animation = animation;
}

Animation* RoboticArm::GetSelectedAnimation() {
	return selected_animation;
}

glm::mat4 RoboticArm::GetLowerBaseTranslationalMatrix() {
	return lower_base.transform.GetTranslationMatrix();
}

glm::mat4 RoboticArm::GetUpperBaseTranslationalMatrix() {
	return upper_base.transform.GetTranslationMatrix();
}

glm::mat4 RoboticArm::GetLowerElbowTranslationalMatrix() {
	return lower_elbow.transform.GetTranslationMatrix();
}

glm::mat4 RoboticArm::GetUpperElbowTranslationalMatrix() {
	return upper_elbow.transform.GetTranslationMatrix();
}

glm::mat4 RoboticArm::GetWristTranslationalMatrix() {
	return wrist.transform.GetTranslationMatrix();
}

glm::mat4 RoboticArm::GetLowerBaseRotationalMatrix() {
	return lower_base.transform.GetRotationMatrix();
}

glm::mat4 RoboticArm::GetUpperBaseRotationalMatrix() {
	return upper_base.transform.GetRotationMatrix();
}

glm::mat4 RoboticArm::GetLowerElbowRotationalMatrix() {
	return lower_elbow.transform.GetRotationMatrix();
}

glm::mat4 RoboticArm::GetUpperElbowRotationalMatrix() {
	return upper_elbow.transform.GetRotationMatrix();
}

glm::mat4 RoboticArm::GetWristRotationalMatrix() {
	return wrist.transform.GetRotationMatrix();
}

glm::mat4 RoboticArm::GetLowerBaseMatrix() {
	return lower_base.matrix;
}

glm::mat4 RoboticArm::GetUpperBaseMatrix() {
	return upper_base.matrix;
}

glm::mat4 RoboticArm::GetLowerElbowMatrix() {
	return lower_elbow.matrix;
}

glm::mat4 RoboticArm::GetUpperElbowMatrix() {
	return upper_elbow.matrix;
}

glm::mat4 RoboticArm::GetWristMatrix() {
	return wrist.matrix;
}

float EaseFunc(float x, float t) {
	return pow(x, t) / (pow(x, t) + pow(1 - x, t));
}

void RoboticArm::AnimationStep() {
	static Position prev_position;
	static float progress = 0;
	static int idx = 0;

	if (animation == nullptr) {
		prev_position = position;

		return;
	}

	if (animation_status == PAUSE) {
		progress = 0;
		prev_position = position;

		return;
	}

	if (this->animation_status == PLAY && this->animation->animation_step_array.size() > 0) {
		this->ik_enable = false;

		if (progress >= 1) {
			for (int i = 0; i < 6; i++)
				prev_position[i] = this->position[i];

			idx++;
			progress = 0;
		} else {
			for (int i = 0; i < 6; i++)
				this->position[i] = prev_position[i] + (this->animation->animation_step_array[idx].position[i] - prev_position[i]) * EaseFunc(progress, animation->animation_step_array[idx].pow_t);

			progress += (1 / animation->animation_step_array[idx].progress_len);
		}
	} else if (this->animation_status != PAUSE) {
		for (int i = 0; i < 6; i++)
			prev_position[i] = this->position[i];
	}

	if (idx >= this->animation->animation_step_array.size()) {
		if (!animation->loop) {
			this->animation_status = STOP;
		} else {
			idx = 0;
		}
	}

	// if (this->animation_status == STOP) {
	// 	idx = 0;
	// 	progress = 0;
	// 	prev_position = position;
	// }
}

void RoboticArm::SaveAnimations() {
	SerializeRoboticArmAnimations("animations.yaml");
}

void RoboticArm::LoadAnimations() {
	DeserializeRoboticArmAnimations("animations.yaml");
}
