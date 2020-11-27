#pragma once
#include <Engine/Window.h>
#include <Engine/Entities/ImGUIManager.h>
#include <Engine/Entities/Model.h>
#include "Camera.h"
#include "PointLight.h"
#include "SkinnedBox.h"
#include <Engine\Architecture\BlurOutlineRenderGraph.h>

import VFileDialog;

class App
{
public:
	App(uint32_t width, uint32_t height);
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

	RG::BlurOutlineRenderGraph rg{ wnd.Gfx() };
	PointLight light;
	std::unique_ptr<Model> model;
	VFileOpenDialog opener;
	float speed = 1.0f;
};

