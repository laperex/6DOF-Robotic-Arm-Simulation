#pragma once

// #define USE_IMGUI_API

#include <vector>
#include <implot.h>
#include <implot_internal.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl.h>
#include <imgui_stdlib.h>
#include <imgui_internal.h>
#include <ImGuizmo.h>

namespace ImGui {
	void SanitisedInputText(const char* label, std::string& text);
	void TransformFloat3(const char* label, float *v, float resetValue = 0.0f, float columnWidth = 100.0f);
	bool in_expections(std::string value, const std::vector<std::string>& exceptions);

	template <typename T>
	void EnumComboLogic(const char* name, const std::vector<std::string>& total_modes, T& idx, const std::vector<std::string>& exceptions = {}, ImGuiComboFlags flags = ImGuiComboFlags_NoArrowButton) {
		if (ImGui::BeginCombo(name, total_modes[idx].c_str(), flags)) {
			for (int i = 0; i < total_modes.size(); i++) {
				if (!in_expections(total_modes[i], exceptions))
					if (total_modes[i] != total_modes[idx])
						if (ImGui::Selectable(total_modes[i].c_str(), true))
							idx = (T)i;
			}
			ImGui::EndCombo();
		}
	}
}