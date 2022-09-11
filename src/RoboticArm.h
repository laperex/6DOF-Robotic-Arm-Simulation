#pragma once

#include "Light.h"
#include "Material.h"

namespace Arm {
	struct Position {
		float base = 90;
		float lower_elbow = 90;
		float upper_elbow = 90;
		float wrist = 90;
		float gripper_rotation = 90;
		float gripper = 90;

		Position() {}
		Position(float base, float lower_elbow, float upper_elbow, float wrist, float gripper_rotation, float gripper);
		float& operator[](int idx) {
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
	};

	enum RoboticArmAnimationStatus {
		PLAY,
		PAUSE,
		STOP,

		RoboticArmAnimationStatus_COUNT
	};

	struct Context {
		Position position;
		
		glm::vec3 ik_target = { 100, 100, 100 };
		bool ik_enable = false;

		std::vector<Position> saved_positions;
		RoboticArmAnimationStatus animation_status = STOP;

		static Context* Instance() {
			static Context instance;
			return &instance;
		}
	};

	void Render(Position position);

	void SetPosition(Position position);
	Arm::Position GetPosition();

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
}
