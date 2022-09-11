#pragma once

#include <vector>

class Editor {
	typedef void(*system_func)();
	std::vector<system_func> systems_array;

	#define self Instance()
	static Editor* Instance() {
		static Editor instance;
		return &instance;
	}

public:
	Editor() {}
	~Editor() {}

	static void AddLayer(system_func func);

	static void Initialize();
	static void RenderBegin();
	static void RenderEnd();
	static void Update();
	static void ShutDown();
};