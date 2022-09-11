#include "Panels.h"
#include <iostream>
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

		ImGui::Spacing();

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
	auto& roboticarm = registry.store<RoboticArm>();

	static UTIL_UUID selected_animation = UTIL_NULL_UUID;

	if (ImGui::Begin("Animation")) {
		if (ImGui::BeginTable("Animation", 3, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders)) {
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text("Name");
			ImGui::TableSetColumnIndex(1);
			ImGui::Text("Play");
			ImGui::TableSetColumnIndex(2);
			ImGui::Text("Loop");

			for (auto& pair: roboticarm.animation_registry) {
				if (pair.second.name.starts_with("_##")) continue;
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				if (ImGui::Selectable(pair.second.name.c_str())) {
					selected_animation = pair.first;
				}

				ImGui::TableSetColumnIndex(1);
				if (ImGui::Button("Play", { ImGui::GetColumnWidth(1), 0 })) {
					roboticarm.animation_status = PLAY;
					roboticarm.animation = pair.second.animation;
				}

				ImGui::TableSetColumnIndex(2);
				if (ImGui::Button((pair.second.animation.loop) ? "True" : "False", { ImGui::GetColumnWidth(2), 0 })) {
					pair.second.animation.loop = !pair.second.animation.loop;
				}
			}

			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			if (ImGui::Button("+", { ImGui::GetColumnWidth(0), 0 })) {
				roboticarm.NewAnimation("");
			}

			ImGui::EndTable();
		}
	}
	ImGui::End();

	if (ImGui::Begin("Animation Property")) {
		if (roboticarm.animation_registry.find(selected_animation) != roboticarm.animation_registry.end()) {
			auto& container = roboticarm.animation_registry[selected_animation];
			auto& animation = container.animation;

			ImGui::SanitisedInputText("Name", container.name);

			if (ImGui::BeginTable("Animation Positions", 9, ImGuiTableFlags_ScrollY | ImGuiTableFlags_Borders)) {
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::Text("Idx");
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("Base");
				ImGui::TableSetColumnIndex(2);
				ImGui::Text("Elbow Lower");
				ImGui::TableSetColumnIndex(3);
				ImGui::Text("Elbow Upper");
				ImGui::TableSetColumnIndex(4);
				ImGui::Text("Wrist");
				ImGui::TableSetColumnIndex(5);
				ImGui::Text("Gripper Rotation");
				ImGui::TableSetColumnIndex(6);
				ImGui::Text("Gripper");
				ImGui::TableSetColumnIndex(7);
				ImGui::Text("Steps");
				ImGui::TableSetColumnIndex(8);
				ImGui::Text("Reset");

				int idx = 0;
				for (auto& animation_step: animation.animation_step_array) {
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					if (ImGui::Selectable(std::to_string(idx++).c_str())) {
						ImGui::OpenPopup("A");
					}

					for (int i = 0; i < 4; i++) {
						ImGui::TableSetColumnIndex(i + 1);
						ImGui::SetNextItemWidth(ImGui::GetColumnWidth(i + 1));
						ImGui::DragFloat(("##" + std::to_string(idx) + std::to_string(i)).c_str(), &animation_step.position[i], 0.1, 0, 180);
					}

					ImGui::TableSetColumnIndex(8);
					if (ImGui::Button(("Reset##" + std::to_string(idx)).c_str(), { ImGui::GetColumnWidth(8), 0 })) {
						animation_step.position = roboticarm.GetPosition();
					}
				}

				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				if (ImGui::Button("+", { ImGui::GetColumnWidth(0), 0 })) {
					animation.animation_step_array.push_back({ roboticarm.GetPosition() });
				}

				ImGui::EndTable();
			}
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
