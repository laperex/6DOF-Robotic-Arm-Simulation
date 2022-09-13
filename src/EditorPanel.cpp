#include "RoboticArm.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include "ECS.h"
#include "Camera.h"
#include "RoboticArm.h"
#include "Window.h"
#include "Events.h"
#include <imgui_lucy_impl.h>

static auto& registry = Registry::Instance();

static Position position;

void EditorPanel() {
	auto& window = registry.store<Window>();
	auto& camera = registry.store<Camera>();
	auto& roboticarm = registry.store<RoboticArm>();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, { 0.0f, 0.0f });

	ImGui::Begin("Editor");
	ImGui::PopStyleVar();

	ImGuizmo::SetOrthographic(false);
	ImGuizmo::SetDrawlist();

	auto pos = ImGui::GetWindowPos();
	auto size = ImGui::GetWindowSize();

	window.pos.x = pos.x;
	window.pos.y = pos.y;
	window.size.x = size.x;
	window.size.y = size.y;

	ImGuizmo::SetRect(pos.x, pos.y, size.x, size.y);

	if (window.framebuffer != nullptr) {
		if (window.framebuffer->width != size.x || window.framebuffer->height != size.y) {
			delete window.framebuffer;
			window.framebuffer = new lgl::FrameBuffer(size.x, size.y, true);
		}
	}

	if (window.framebuffer != nullptr) {
		ImGui::GetWindowDrawList()->AddImage((void*)window.framebuffer->texture->id, { pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { 0, 1 }, { 1, 0 });
	}

	// window.framebuffer->Bind();

	// static int selected_id = 0;
	// auto pixel = glm::vec4(0.0);
	// if (Events::IsButtonPressed(SDL_BUTTON_LEFT)) {
	// 	auto norm = (Events::GetCursorPosNormalized(window.pos.x, window.pos.y, window.size.x, window.size.y) * glm::vec3(window.size.x, window.size.y, 0) + glm::vec3(window.size.x, window.size.y, 0)) / 2.0f;

	// 	lgl::SetReadBuffer(lgl::Attachment::COLOR_ATTACHMENT1);
	// 	lgl::ReadPixels(norm.x, norm.y, 1, 1, lgl::Format::RGBA, lgl::Type::FLOAT, &pixel[0]);
	// 	lgl::ResetReadBuffer();
	// }

	// window.framebuffer->UnBind();

	ImGui::End();
}

Position GetGizmoPosition() {
	return position;
}
