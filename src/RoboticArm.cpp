#include "RoboticArm.h"
#include <LucyRE/LucyRE.h>
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

void Arm::Render(Position position) {
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

#define self Context::Instance()

void Arm::SetPositon(Position position) {
	self->position = position;
}

glm::vec3 Arm::GetPositon() {
	return self->position;
}
