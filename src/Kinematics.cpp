#include "Kinematics.h"

Arm::Position Arm::GetInverseKinematics(glm::vec3 target, float len_elbow_lower, float len_elbow_upper, float len_wrist) {
	float b = len_elbow_lower;
	float d = len_elbow_upper;
	float a = len_wrist;

	auto [x, y, z] = target;

	float base_angle = atan2(x, z) * TO_DEGREE;
	float l = sqrt(x * x + z * z);

	float c = sqrt(l * l + y * y);
	float f = c - b;

	float A = acos((b * b + f * f - a * a) / (2 * b * f)) * TO_DEGREE;
	float B = acos((f * f + a * a - b * b) / (2 * f * a)) * TO_DEGREE;
	float C = acos((a * a + b * b - f * f) / (2 * a * b)) * TO_DEGREE;

	float theta = atan2(y, l) * TO_DEGREE;
	
	return { base_angle, A + theta, C + B, 180 - B, 0, 0 };
}

glm::vec3 Arm::GetForwardKinematics(Position position, float len_elbow_lower, float len_elbow_upper, float len_wrist) {
	return { 0, 0, 0 };
}
