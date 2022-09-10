#pragma once

#include <LucyRE/LucyRE.h>

struct Camera {
	glm::mat4 projection;
	glm::mat4 view;

	glm::vec3 rotation = { 0, 0, 0 };

	glm::vec3 position = { 0.0, 0.0, 0.0 };
	glm::vec3 offset = { 0.0, 0.0, 0.0 };
	glm::vec3 world_front = { 0.0, 0.0, -1.0 };
	glm::vec3 front = world_front;
	glm::vec3 world_up = { 0.0, 1.0, 0.0 };
	glm::vec3 up = world_up;

	glm::vec4 clear_color = { 0, 0, 0, 1 };
	uint32_t clear_flags = lgl::COLOR_BUFFER_BIT | lgl::DEPTH_BUFFER_BIT;

	int width, height;
	int posx, posy;
	int lastx, lasty;
	bool first_mouse = true;
	float fov = 45;
	float c_near = 0.01, c_far = 1000;
	float sensitivity = 0.5;

	bool enable = true;

	Camera();
	void Update(double dt);
};