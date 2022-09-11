#include "Panels.h"
#include <imgui_lucy_impl.h>
#include <glm/glm.hpp>
#include <LucyRE/LucyRE.h>
#include "Material.h"
#include "Light.h"
#include "ECS.h"
#include "RoboticArm.h"

static auto& registry = Registry::Instance();

void ArmControlPanel() {
	auto& roboticarm = registry.store<RoboticArm>();

	if (ImGui::Begin("Arm Control")) {
		auto pos = roboticarm.GetPosition();
	
		ImGui::DragFloat("Base Y", &pos.base, 0.1, 0, 180);
		ImGui::DragFloat("Lower Elbow", &pos.lower_elbow, 0.1, 0, 180);
		ImGui::DragFloat("Upper Elbow", &pos.upper_elbow, 0.1, 0, 180);
		ImGui::DragFloat("Wrist", &pos.wrist, 0.1, 0, 180);

		roboticarm.SetPosition(pos);

		auto target = roboticarm.GetIKTarget();

		ImGui::Checkbox("IK Enable", &roboticarm.IKEnable());

		ImGui::DragFloat("Upper Elbow Length", &roboticarm.UpperElbowLength(), 0.1, 0);
		ImGui::DragFloat("Lower Elbow Length", &roboticarm.LowerElbowLength(), 0.1, 0);
		ImGui::DragFloat("Wrist Length", &roboticarm.WristLength(), 0.1, 0);

		ImGui::DragFloat3("Target", &target[0], 0.1);

		roboticarm.SetIKTarget(target);

		if (ImGui::Button("Clear")) {
			roboticarm.ClearSavedPositions();
		}

		if (ImGui::Button("Save")) {
			roboticarm.AddPosition(roboticarm.GetPosition());
		}

		ImGui::Spacing();

		auto& animation_status = roboticarm.AnimationStatus();

		switch (animation_status) {
			case PLAY:
				ImGui::TextColored({ 0, 1, 0, 1 }, "PLAYING");
				break;
			case PAUSE:
				ImGui::TextColored({ 1, 1, 0, 1 }, "PAUSED");
				break;
			case STOP:
				ImGui::TextColored({ 1, 0, 0, 1 }, "STOPPED");
				break;

			default:	
				break;
		}

		if (ImGui::Button("PLAY")) {
			animation_status = PLAY;
		}

		if (ImGui::Button("PAUSE")) {
			animation_status = PAUSE;
		}

		if (ImGui::Button("STOP")) {
			animation_status = STOP;
		}
	}
	ImGui::End();
}

void AnimationEditPanel() {
	if (ImGui::Begin("Animation Properties")) {
		if (ImGui::BeginTable("Animation", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders)) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Name");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Duration");
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("Steps");

			// for (auto& pair: ania) {
			// 	if (pair.second.name.starts_with("_##")) continue;

			// 	ImGui::TableNextRow();
			// }

			ImGui::EndTable();
		}
	}
	ImGui::End();
}

void MaterialLightEditPanel() {
	if (ImGui::Begin("Lighting and Material")) {
		auto& light = registry.store<Light>();
		auto& material = registry.store<Material>();

		if (ImGui::TreeNodeEx("Material", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat3("Ambient", &material.ambient[0], 0.1, 0, 1);
			ImGui::DragFloat3("Diffuse", &material.diffuse[0], 0.1, 0, 1);
			ImGui::DragFloat3("Specular", &material.specular[0], 0.1, 0, 1);
			ImGui::DragFloat("Shininess", &material.shininess, 0.1, 0, 32);
		
			ImGui::TreePop();
		}

		if (ImGui::TreeNodeEx("Light", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::DragFloat("Ambient", &light.ambient, 0.1, 0, 1);
			ImGui::DragFloat("Diffuse", &light.diffuse, 0.1, 0, 1);
			ImGui::DragFloat("Specular", &light.specular, 0.1, 0, 1);

			ImGui::ColorEdit4("Color", &light.color[0], ImGuiColorEditFlags_NoInputs);

			ImGui::TreePop();
		}
	}
	ImGui::End();
}
