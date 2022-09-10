#pragma once

namespace lre {	
	struct DirectionalLight {
		glm::vec3 color = { 1, 1, 1 };
		glm::vec3 direction = { 0, 1, 0 };

		float intensity;
	};

	struct PointLight {
		glm::vec3 color = { 1, 1, 1 };
		glm::vec3 position = { 0, 0, 0 };

		float intensity;
	};

	struct SpotLight {
		glm::vec3 color = { 1, 1, 1 };

		float radius;
		float angle;
	};
}