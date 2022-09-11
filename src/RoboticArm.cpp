#include "RoboticArm.h"
#include <LucyRE/LucyRE.h>
#include <iostream>
#include "Transform.h"

struct Node {
	glm::vec3 position = { 0, 0, 0 };
	UTIL_UUID mesh_id = UTIL_NULL_UUID;
	Transform transform;
};

static Node lower_base = {{ 44.0448, 71.6270, 55.25 }};
static Node upper_base = {{ 44.0448, 71.6270, 55.25 }};
static Node lower_elbow = {{ 44.2948, 86.6948, 55.25 }};
static Node upper_elbow = {{ 44.2948, 190.8709, 55.25 }};
static Node wrist = {{ 43.2270, 285.2090, 55.25 }};

void RoboticArm::Render(Position position) {
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

	auto base_rotation = upper_base.transform.GetRotationMatrix();

	lre::SetModel(lower_base.transform.GetTranslationMatrix());
	lre::RenderMesh(lower_base.mesh_id);
	lre::SetModel(base_rotation * upper_base.transform.GetTranslationMatrix());
	lre::RenderMesh(upper_base.mesh_id);
	lre::SetModel(base_rotation * lower_elbow.transform.GetTranslationMatrix() * lower_elbow.transform.GetRotationMatrix());
	lre::RenderMesh(lower_elbow.mesh_id);
	lre::SetModel(base_rotation * upper_elbow.transform.GetTranslationMatrix() * upper_elbow.transform.GetRotationMatrix());
	lre::RenderMesh(upper_elbow.mesh_id);
	lre::SetModel(base_rotation * wrist.transform.GetTranslationMatrix() * wrist.transform.GetRotationMatrix());
	lre::RenderMesh(wrist.mesh_id);
}

void RoboticArm::SetPosition(Position position) {
	this->position = position;
}

Position RoboticArm::GetPosition() {
	return position;
}

float& RoboticArm::UpperElbowLength() {
	static float len = 120.59;
	return len;
}

float& RoboticArm::LowerElbowLength() {
	static float len = 94.34;
	return len;
}

float& RoboticArm::WristLength() {
	static float len = 121.86;
	return len;
}

bool& RoboticArm::IKEnable() {
	return ik_enable;
}

void RoboticArm::SetIKTarget(glm::vec3 target) {
	ik_target = target;
}

glm::vec3 RoboticArm::GetIKTarget() {
	return ik_target;
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

void RoboticArm::AddPosition(Position position) {
	saved_positions.push_back(position);
}

void RoboticArm::ClearSavedPositions() {
	saved_positions.clear();
}

RoboticArmAnimationStatus& RoboticArm::AnimationStatus() {
	return animation_status;
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

Position::Position(float base, float lower_elbow, float upper_elbow, float wrist, float gripper_rotation, float gripper) {
	this->base = base;
	this->lower_elbow = lower_elbow;
	this->upper_elbow = upper_elbow;
	this->wrist = wrist;
	this->gripper_rotation = gripper_rotation;
	this->gripper = gripper;
}

float& Position::operator[](int idx) {
	assert(idx >= 0 && idx <= 5);

	switch (idx) {
		case 0:
			return base;
			break;
		case 1:
			return lower_elbow;
			break;
		case 2:
			return upper_elbow;
			break;
		case 3:
			return wrist;
			break;
		case 4:
			return gripper_rotation;
			break;
		case 5:
			return gripper;
			break;
	}
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
		this->animation_status = STOP;
	}

	if (this->animation_status == STOP) {
		idx = 0;
		progress = 0;
		prev_position = position;
	}
}
