#include "RoboticArm.h"
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
		ImGui::GetWindowDrawList()->AddImage((void*)window.framebuffer->texture->id, { pos.x, pos.y }, { pos.x + size.x, pos.y + size.y }, { 0, window.size.y / window.framebuffer->height }, { window.size.x / window.framebuffer->width, 0 });
	}

	static int selected_id = 0;
	auto pixel = roboticarm.GetPixel();

	if (pixel[0] == 1.0f) {
		selected_id = pixel[1];
	}

	if (selected_id != 0) {
		if (selected_id == 2) {
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Y, ImGuizmo::LOCAL, &roboticarm.GetUpperBaseMatrix()[0][0]);
		}
		if (selected_id == 3) {
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Z, ImGuizmo::LOCAL, &roboticarm.GetLowerElbowMatrix()[0][0]);
		}
		if (selected_id == 4) {
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Z, ImGuizmo::LOCAL, &roboticarm.GetUpperElbowMatrix()[0][0]);
		}
		if (selected_id == 5) {
			ImGuizmo::Manipulate(&camera.view[0][0], &camera.projection[0][0], ImGuizmo::ROTATE_Z, ImGuizmo::LOCAL, &roboticarm.GetWristMatrix()[0][0]);
		}
	}

	ImGui::End();
}

Position GetGizmoPosition() {
	return position;
}
