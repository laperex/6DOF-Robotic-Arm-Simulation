#include "Events.h"
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

static auto& registry = Registry::Instance();

int main(int argcount, char** args) {
	SDL_Init(SDL_INIT_VIDEO);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	auto& window = registry.store<Window>();
	auto& camera = registry.store<Camera>();

	window.InitSDLWindow();
	lgl::Initialize(SDL_GL_GetProcAddress);
	lre::Initialize();

	double dt = 0;
	while (!Events::IsQuittable()) {
		const auto& start_time = std::chrono::high_resolution_clock::now();

		Events::Update();

		camera.width = window.size.x;
		camera.height = window.size.y;

		camera.Update(dt);

		lgl::Clear(0, 0, 0, 1, lgl::COLOR_BUFFER_BIT | lgl::DEPTH_BUFFER_BIT);
		lgl::Viewport(0, 0, window.size.x, window.size.y);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LINE_SMOOTH);

		lre::SetProjection(camera.projection);
		lre::SetView(camera.view);
		lre::SetViewPosition(camera.position);

		auto* shader = lre::GetShader("phong");
		shader->Bind();


		Arm::Render({ 90, 90, 90, 90, 90, 90 });

		shader->UnBind();

		window.SwapWindow();

		const auto& end_time = std::chrono::high_resolution_clock::now();
		dt = std::chrono::duration<double, std::ratio<1, 60>>(end_time - start_time).count();
	}

	return 0;
}
