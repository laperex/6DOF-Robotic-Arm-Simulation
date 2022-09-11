#pragma once

#include <glm/glm.hpp>
#include "RoboticArm.h"

Position GetInverseKinematics(glm::vec3 target, float len_elbow_lower, float len_elbow_upper, float len_wrist);
glm::vec3 GetForwardKinematics(Position position, float len_elbow_lower, float len_elbow_upper, float len_wrist);