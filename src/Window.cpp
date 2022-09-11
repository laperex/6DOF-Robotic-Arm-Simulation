#include "Window.h"

Window::Window() {
	this->pos = { 0, 0 };
	this->size = { 1920 * .8, 1080 * .8 };

	this->flags |= SDL_WINDOW_BORDERLESS;
}

Window::~Window() {
	Destroy();
}

void Window::InitSDLWindow() {
	assert(this->sdl_window == nullptr);

	this->sdl_window = SDL_CreateWindow(this->title.c_str(), this->pos.x, this->pos.y, this->size.x, this->size.y, this->flags);

	assert(this->sdl_window);

	this->sdl_glcontext = SDL_GL_CreateContext(this->sdl_window);
}

void Window::InitSDLWindow(const char* title, int pos_x, int pos_y, int width, int height, uint32_t flags) {
	this->pos.x = pos_x;
	this->pos.y = pos_y;
	this->size.x = width;
	this->size.y = height;
	this->title = std::string(title);
	this->flags |= flags;

	InitSDLWindow();
}

void Window::SwapWindow() {
	SDL_GL_SwapWindow(this->sdl_window);
}

void Window::Destroy() {
	if (this->framebuffer != nullptr) {
		delete this->framebuffer;
		this->framebuffer = nullptr;
	}

	SDL_DestroyWindow(this->sdl_window);
}

void Window::SetCurrent() {
	SDL_GL_MakeCurrent(this->sdl_window, this->sdl_glcontext);
}

void Window::ResetSize() {
	int w, h;
	SDL_GetWindowSize(this->sdl_window, &w, &h);
	this->size = { w, h };
}

void Window::ResetPosition() {
	int x, y;
	SDL_GetWindowPosition(this->sdl_window, &x, &y);
	this->pos = { x, y };
}

void Window::Hide() {
	if (!this->hidden)
		SDL_HideWindow(this->sdl_window);
	this->hidden = true;
}

void Window::Show() {
	if (this->hidden)
		SDL_ShowWindow(this->sdl_window);
	this->hidden = false;
}
