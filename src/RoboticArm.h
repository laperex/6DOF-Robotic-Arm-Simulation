#pragma once

#include "LucyUtil/UUID.h"
#include "Light.h"
#include "Material.h"

struct Position {
	float base = 90;
	float lower_elbow = 90;
	float upper_elbow = 90;
	float wrist = 90;
	float gripper_rotation = 90;
	float gripper = 90;

	Position() {}
	Position(float base, float lower_elbow, float upper_elbow, float wrist, float gripper_rotation, float gripper);
	float& operator[](int idx);
};

struct AnimationStep {
	Position position;
	float progress_len = 2000;	// Steps taken to reach this position
	float pow_t = 1.9;	// Step Gradient

	AnimationStep() {}
	AnimationStep(Position position): position(position) {}
};

struct Animation {
	std::vector<AnimationStep> animation_step_array;

	float delay = 0;	// Delay after reaching position
	bool loop = false;
};

enum RoboticArmAnimationStatus {
	PLAY,
	PAUSE,
	STOP,

	RoboticArmAnimationStatus_COUNT
};

struct RoboticArm {
	Position position;
	
	glm::vec3 ik_target = { 100, 100, 100 };
	bool ik_enable = false;

	std::vector<Position> saved_positions;
	RoboticArmAnimationStatus animation_status = STOP;
	Animation* animation = nullptr;

	struct AnimationContainer {
		std::string name;
		Animation animation;
	};

	std::unordered_map<UTIL_UUID, AnimationContainer> animation_registry;

	Animation* selected_animation = nullptr;

	void Render(Position position);

	void SetPosition(Position position);
	Position GetPosition();

	float& UpperElbowLength();
	float& LowerElbowLength();
	float& WristLength();

	bool& IKEnable();
	void SetIKTarget(glm::vec3 target);
	glm::vec3 GetIKTarget();

	void SetLight(Light light);
	void SetMaterial(Material light);
	Light SetLight();
	Material SetMaterial();

	void AddPosition(Position position);
	void ClearSavedPositions();
	RoboticArmAnimationStatus& AnimationStatus();

	void Step();
	void AnimationStep();

	UTIL_UUID NewAnimation(std::string name, Animation animation = {}, UTIL_UUID id = UTIL_GENERATE_UUID);
	Animation* GetAnimation(UTIL_UUID id);
	UTIL_UUID GetAnimationIDByName(std::string name = "");
	void SetSelectedAnimation(Animation* animation);
	Animation* GetSelectedAnimation();

private:
	bool IsNamePresent(std::string name);
	std::string GetName(std::string name);
};

