#include "RoboticArm.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <iostream>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include "ECS.h"
#include "Camera.h"
#include "RoboticArm.h"
#include "Window.h"
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

	static int selected_id = 0;
	auto pixel = roboticarm.GetPixel();

	if (pixel[0] == 1.0f && !ImGuizmo::IsUsing() && !ImGuizmo::IsOver()) {
		selected_id = pixel[1];
	}

	if (selected_id != 0) {
		if (selected_id == 2) {
			auto model = roboticarm.GetUpperBaseTranslationalMatrix() * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, roboticarm.position.base, 0))));
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Y, ImGuizmo::WORLD, &model[0][0]);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;

				ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &translation[0], &rotation[0], &scale[0]);

				roboticarm.position.base = rotation.y;
			}
		}
		if (selected_id == 3) {
			auto model = roboticarm.GetLowerElbowTranslationalMatrix() * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, 0, roboticarm.position.lower_elbow))));
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Z, ImGuizmo::WORLD, &model[0][0]);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;

				ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &translation[0], &rotation[0], &scale[0]);

				roboticarm.position.lower_elbow = rotation.z;
			}
		}
		if (selected_id == 4) {
			auto model = roboticarm.GetUpperElbowTranslationalMatrix() * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, 0, roboticarm.position.upper_elbow))));
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Z, ImGuizmo::WORLD, &model[0][0]);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;

				ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &translation[0], &rotation[0], &scale[0]);

				roboticarm.position.upper_elbow = rotation.z;
			}
		}
		if (selected_id == 5) {
			auto model = roboticarm.GetWristTranslationalMatrix() * glm::toMat4(glm::quat(glm::radians(glm::vec3(0, 0, roboticarm.position.wrist))));
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Z, ImGuizmo::WORLD, &model[0][0]);

			if (ImGuizmo::IsUsing()) {
				glm::vec3 translation, rotation, scale;

				ImGuizmo::DecomposeMatrixToComponents(&model[0][0], &translation[0], &rotation[0], &scale[0]);

				roboticarm.position.wrist = rotation.z;
			}
		}
	}

	ImGui::End();
}

Position GetGizmoPosition() {
	return position;
}
