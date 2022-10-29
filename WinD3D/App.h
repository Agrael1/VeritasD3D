#pragma once
#include <Entities/VFileDialog.h>
#include <Window.h>
#include <Framework/ImGUIManager.h>
#include <Entities/Model.h>
#include <Scene/Camera.h>
#include <Scene/PointLight2.h>
#include <Scene/Grid.h>
#include <Architecture/BlurOutlineRenderGraph.h>
#include <DeferredRenderGraph.h>
#include <Probes/TestModelProbe.h>
#include <Architecture/Light.h>
#include "Text.h"
#include <Scene/PointLight.h>
#include <Architecture/GalaxyRenderGraph.h>

class App
{
public:
	App(uint32_t width, uint32_t height);
	~App();
	winrt::IAsyncAction InitializeAsync();
public:
	int Go();
private:
	void DoFrame(float dt);
	void ProcessInput(float dt);
	void CreateRenderGraph();
	winrt::fire_and_forget
		ReloadModelAsync();
private:
	ImGUIManager imgui;
	Window wnd;
	Camera cam;

	//std::optional<RG::GalaxyRenderGraph> rg;
	std::optional<RG::DeferredRenderGraph> rg;
	//PointLight light;

	//Grid grid;
	std::unique_ptr<Model> model, swap;
	VFileOpenDialog opener;
	float speed = 1.0f;

	MP modelProbe;

	enum class ModelLoadState : uint8_t
	{
		Unloaded,
		InProgress,
		Finish
	};
	std::atomic<ModelLoadState> state = ModelLoadState::Unloaded;
	bool bFlightMode = false;

	ver::Text text;
	//ver::SolidSphere ss;
	ver::LightBuffer lights;
	ver::LightSphere sphere;
};

