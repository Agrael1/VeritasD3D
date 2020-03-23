#pragma once
#include <Engine/Window.h>
#include <Engine/Entities/ImGUIManager.h>
#include <Engine/Entities/Model.h>
#include "Camera.h"
#include "PointLight.h"
#include "SkinnedBox.h"
#include <Engine/Architecture/FrameCommander.h>

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
	FrameCommander fc;

	PointLight light;
	//TestCube cube{ wnd.Gfx(),4.0f };
	//TestCube cube2{ wnd.Gfx(),4.0f };
	Model sponza{ wnd.Gfx(), "Models\\brick_wall\\brick_wall.obj", 1.0f/*/20.0f*/ };

	float speed = 1.0f;
};

