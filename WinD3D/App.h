#pragma once
#include "Engine\Window.h"
#include "ImGUIManager.h"
#include "Camera.h"

class App
{
public:
	App();
	~App();
public:
	int Go();
private:
	void DoFrame(float dt);
private:
	ImGUIManager imgui;
	Window wnd;
	Camera cam;

	float speed = 1.0f;
	std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
};

