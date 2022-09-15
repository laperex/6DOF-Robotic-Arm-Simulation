#include "Events.h"
#include "Kinematics.h"
#include "EditorLayer.h"
#include "Transform.h"
#include "Camera.h"
#include "Window.h"
#include <chrono>
#include <LucyGL/LucyGL.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <ECS.h>
#include <LucyRE/LucyRE.h>
#include <LucyUtil/Importer.h>
#include "RoboticArm.h"
#include "Panels.h"
#include "EditorPanel.h"

static auto& registry = Registry::Instance();

int main(int argcount, char** args) {
	// Editor::AddLayer(RoboticArmGizmoSystem);
	Editor::AddLayer(EditorPanel);
	Editor::AddLayer(ArmControlPanel);
	Editor::AddLayer(AnimationEditPanel);

	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	auto& window = registry.store<Window>();
	auto& camera = registry.store<Camera>();

	window.InitSDLWindow();

	lgl::Initialize(SDL_GL_GetProcAddress);
	lre::Initialize();

	Editor::Initialize();

	auto& light = registry.store<Light>();
	auto& material = registry.store<Material>();

	auto& roboticarm = registry.store<RoboticArm>();

	roboticarm.LoadAnimations();

	double dt = 0;
	while (!Events::IsQuittable()) {
		const auto& start_time = std::chrono::high_resolution_clock::now();

		Events::Update();

		glEnable(GL_DEPTH_TEST);
		lgl::Viewport(0, 0, window.size.x, window.size.y);
		lgl::Clear(0, 0, 0, 1, lgl::COLOR_BUFFER_BIT | lgl::DEPTH_BUFFER_BIT);

		if (window.framebuffer == nullptr) {
			window.framebuffer = new lgl::FrameBuffer(window.size.x, window.size.y, true);
		}

		window.framebuffer->Bind();
		lre::SetFrameBuffer(window.framebuffer);

		// glEnable(GL_DEPTH_TEST);
		// glEnable(GL_LINE_SMOOTH);
		// lgl::Viewport(0, 0, window.size.x, window.size.y);
		// lgl::Clear(0, 0, 0, 1, lgl::COLOR_BUFFER_BIT | lgl::DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);
		glViewport(0, 0, window.size.x, window.size.y);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		camera.width = window.size.x;
		camera.height = window.size.y;

		camera.Update(dt);

		lre::SetProjection(camera.projection);
		lre::SetView(camera.view);
		lre::SetViewPosition(camera.position);

		auto* shader = lre::GetShader("phong");

		light.Bind(shader);
		material.Bind(shader);

		{
			if (roboticarm.ik_enable)
				roboticarm.position = GetInverseKinematics(roboticarm.ik_target, roboticarm.lowerelbow_length, roboticarm.upperelbow_length, roboticarm.wrist_length);

			roboticarm.AnimationStep();
			roboticarm.CalculateMatrices(roboticarm.position);
			roboticarm.Render();
		}

		shader->UnBind();

		{
			lre::SetModel(glm::mat4(1.0));
			
			glEnable(GL_LINE_SMOOTH);

			int unit_size = 100;
			int grid_len = 10;

			static std::vector<glm::vec3> grid;

			// for (int z = -(grid_len / 2); z < +(grid_len / 2); z++) {
			// 	grid.push_back({ -(grid_len / 2) * unit_size, 0, z * unit_size });
			// 	grid.push_back({ +(grid_len / 2) * unit_size, 0, z * unit_size });
			// }
			lre::GridTest(window.framebuffer, window.size);

			lre::RenderLine(grid, { 1, 1, 1, 1 });

			lre::RenderLine({ 0, 0, 0 }, { 0, 0, 10000 }, { 0, 0, 1, 0.7 });
			lre::RenderLine({ 0, 0, 0 }, { 0, 10000, 0 }, { 0, 1, 0, 0.7 });
			lre::RenderLine({ 0, 0, 0 }, { 10000, 0, 0 }, { 1, 0, 0, 0.7 });

			lre::FlushLine();
		}

		window.framebuffer->UnBind();

		Editor::Update();

		if (Events::IsKeyPressed({ SDL_SCANCODE_S, SDL_SCANCODE_LCTRL })) {
			roboticarm.SaveAnimations();
		}

		// lre::RenderFrameBufferToScreen(window.framebuffer, window.size);

		window.SwapWindow();

		const auto& end_time = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<double, std::ratio<1, 60>>(end_time - start_time).count();
	}

	lre::Destroy();
	Editor::ShutDown();

	return 0;
}
