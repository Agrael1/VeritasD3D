#pragma once
#include <Entities/VFileDialog.h>
#include <Window.h>
#include <Framework/ImGUIManager.h>
#include <Entities/Model.h>
#include <Scene/Camera.h>
#include <Scene/PointLight.h>
#include <Scene/Grid.h>
#include <Architecture\BlurOutlineRenderGraph.h>


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
	void CreateRenderGraph();
private:
	ImGUIManager imgui;
	Window wnd;
	Camera cam;

	std::optional<RG::BlurOutlineRenderGraph> rg;
	PointLight light;

	Grid grid;
	std::unique_ptr<Model> model;
	VFileOpenDialog opener;
	float speed = 1.0f;
};

