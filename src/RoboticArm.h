#pragma once

#include "LucyUtil/UUID.h"
#include "Light.h"
#include "Material.h"
#include "Position.h"

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

	RoboticArmAnimationStatus animation_status = STOP;
	Animation* animation = nullptr;

	float upperelbow_length = 120.59;
	float lowerelbow_length = 94.34;
	float wrist_length = 121.86;

	struct AnimationContainer {
		std::string name;
		Animation animation;
	};

	std::unordered_map<UTIL_UUID, AnimationContainer> animation_registry;

	Animation* selected_animation = nullptr;

	void CalculateMatrices(Position position);
	void Render();
	glm::vec4 GetPixel();
	glm::vec4 RenderPicking();
	void AnimationStep();

	UTIL_UUID NewAnimation(std::string name, Animation animation = {}, UTIL_UUID id = UTIL_GENERATE_UUID);
	Animation* GetAnimation(UTIL_UUID id);
	UTIL_UUID GetAnimationIDByName(std::string name = "");
	void SetSelectedAnimation(Animation* animation);
	Animation* GetSelectedAnimation();

	glm::mat4 GetLowerBaseMatrix();
	glm::mat4 GetUpperBaseMatrix();
	glm::mat4 GetLowerElbowMatrix();
	glm::mat4 GetUpperElbowMatrix();
	glm::mat4 GetWristMatrix();

	glm::mat4 GetLowerBaseTranslationalMatrix();
	glm::mat4 GetUpperBaseTranslationalMatrix();
	glm::mat4 GetLowerElbowTranslationalMatrix();
	glm::mat4 GetUpperElbowTranslationalMatrix();
	glm::mat4 GetWristTranslationalMatrix();
	
	glm::mat4 GetLowerBaseRotationalMatrix();
	glm::mat4 GetUpperBaseRotationalMatrix();
	glm::mat4 GetLowerElbowRotationalMatrix();
	glm::mat4 GetUpperElbowRotationalMatrix();
	glm::mat4 GetWristRotationalMatrix();

	void SaveAnimations();
	void LoadAnimations();

private:
	bool IsNamePresent(std::string name);
	std::string GetName(std::string name);
};
