#pragma once
//#include <Engine/Entities/VFileDialog.h>
//#include <Engine/Window.h>
//#include <Engine/Entities/ImGUIManager.h>
//#include <Engine/Entities/Model.h>
//#include "Camera.h"
//#include "PointLight.h"
//#include <Engine\Architecture\BlurOutlineRenderGraph.h>
//#include "Grid.h"
//
//class VApp
//{
//public:
//	VApp(uint32_t width, uint32_t height);
//	~VApp();
//public:
//	int Go();
//private:
//	void DoFrame(float dt);
//	void ProcessInput(float dt);
//private:
//	ImGUIManager imgui;
//	Window wnd;
//	Camera cam;
//
//	RG::BlurOutlineRenderGraph rg{ wnd.Gfx() };
//	PointLight light;
//
//	Grid grid;
//	std::unique_ptr<Model> model;
//	VFileOpenDialog opener;
//	float speed = 1.0f;
//};