#pragma once
#include <Engine\Window.h>
#include "ImGUIManager.h"
#include "Camera.h"
#include "Model.h"
#include "PointLight.h"

class App
{
public:
	App();
	~App();
public:
	int Go();
private:
	void DoFrame(float dt);
	void ProcessInput(float dt);
private:
	ImGUIManager imgui;
	Window wnd;
	Camera cam;
	PointLight light;
	Model nano{ wnd.Gfx(), "Models\\nano_textured\\nanosuit.obj" };

	float speed = 1.0f;
	//std::vector<std::unique_ptr<class Drawable>> drawables;
	static constexpr size_t nDrawables = 180;
};

