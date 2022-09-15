#include "Camera.h"
#include "Events.h"
#include "ECS.h"
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

static auto& registry = Registry::Instance();

glm::vec2 last_size = { 0, 0 };
float scrollspeed = 0.75 * 3;
bool toggle = false;

glm::vec3 delta;
glm::vec3 initpos;

Camera::Camera() {
	position = { 0, 0, 500 };
	c_near = 10;
	c_far = 3000;
}

void Camera::Update(double dt) {
	auto norm_cursor_pos = Events::GetCursorPosNormalized(0, 0, this->width, this->height);
	auto cursor_pos = Events::GetCursorPos();

	if (this->width != last_size.x || this->height != last_size.y) {
		last_size.x = this->width;
		last_size.y = this->height;

		this->lastx = this->width / 2;
		this->lasty = this->height / 2;

		this->first_mouse = true;

		this->projection = glm::perspective(glm::radians(this->fov), (float)this->width / this->height, this->c_near, this->c_far);
	}

	if (Events::IsButtonPressed(SDL_BUTTON_MIDDLE)) {
		if (this->first_mouse) {
			this->lastx = cursor_pos.x;
			this->lasty = cursor_pos.y;
			this->first_mouse = false;
		}

		this->rotation.y -= (cursor_pos.x - this->lastx) * this->sensitivity;
		this->rotation.x += (this->lasty - cursor_pos.y) * this->sensitivity;

		this->lastx = cursor_pos.x;
		this->lasty = cursor_pos.y;
	} else {
		this->first_mouse = true;
	}

	const auto& quaternion = glm::quat(glm::radians(this->rotation));

	this->front = glm::normalize(quaternion * this->world_front);
	this->up = glm::normalize(quaternion * this->world_up);

	if (Events::IsMouseScrollingUp())
		this->position += this->front * float(scrollspeed * dt);
	if (Events::IsMouseScrollingDown())
		this->position -= this->front * float(scrollspeed * dt);

	float distance = glm::length(this->position);
	scrollspeed = distance / 10;
	this->position = distance * -this->front;

	if (Events::IsButtonPressed(SDL_BUTTON_LEFT) && Events::IsKeyPressed(SDL_SCANCODE_LALT)) {
		glm::vec4 ray_clip = glm::vec4(norm_cursor_pos.x, norm_cursor_pos.y, -1.0, 1.0);
		glm::vec4 ray_eye = glm::inverse(this->projection) * ray_clip;
		ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);
		glm::vec3 ray_wor = glm::inverse(this->view) * ray_eye;
		ray_wor = glm::normalize(ray_wor);

		float denom = glm::dot(this->front, ray_wor);
		float t = -glm::dot((this->front * distance), this->front) / denom;
		glm::vec3 pos = ray_wor * t + (this->offset + this->position);

		if (!toggle)
			initpos = pos;

		delta = pos - initpos;
		toggle = true;
	}

	this->view = glm::lookAt(this->position + this->offset + delta, this->position + this->offset + delta + this->front, this->up);

	if (toggle && !Events::IsButtonPressed(SDL_BUTTON_LEFT) && Events::IsKeyPressed(SDL_SCANCODE_LALT)) {
		this->offset += delta;
		toggle = false;
		delta = glm::vec3();
	}
}

