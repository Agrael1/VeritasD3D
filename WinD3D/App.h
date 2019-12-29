#pragma once
#include <Engine/Window.h>
#include <Engine/Entities/ImGUIManager.h>
#include <Engine/Entities/Model.h>
#include "Camera.h"
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
	Model Sponza{ wnd.Gfx(), "Models\\Sponza\\sponza.obj", 1.0f/20.0f };

	float speed = 1.0f;
	static constexpr size_t nDrawables = 180;
};

