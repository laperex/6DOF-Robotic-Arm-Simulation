#pragma once

namespace Arm {
	struct Position {
		float base = 90;
		float lower_elbow = 90;
		float upper_elbow = 90;
		float wrist = 90;
		float gripper_rotation = 90;
		float gripper = 90;

		Position() {}
		Position(float base, float lower_elbow, float upper_elbow, float wrist, float gripper_rotation, float gripper) {
			this->base = base;
			this->lower_elbow = lower_elbow;
			this->upper_elbow = upper_elbow;
			this->wrist = wrist;
			this->gripper_rotation = gripper_rotation;
			this->gripper = gripper;
		}
	};

	void Render(Position position);
}