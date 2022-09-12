#include "Position.h"
#include <assert.h>

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
