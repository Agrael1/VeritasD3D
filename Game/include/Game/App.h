#pragma once
#include <Engine/Util/VFileDialog.h>
#include <Game/Window.h>
#include <Game/Camera.h>
#include <Engine/Util/ImGUIManager.h>
#include <Engine/Loading/Model.h>
#include <Engine/Scene/PointLight2.h>
#include <Engine/Graph/DeferredRenderGraph.h>
#include <Engine/Probes/TestModelProbe.h>
#include <Engine/Bindable/Light.h>
#include <Engine/Scene/Text.h>
#include <Game/Player.h>
#include <Game/Scene.h>
#include <Foundation.h>

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
	//Camera cam;
	ImGUIManager imgui;
	Window wnd;
	Graphics gfx;

	std::optional<RG::DeferredRenderGraph> rg;

	//std::unique_ptr<Model> model, swap;
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

	//ver::Text text;
	ver::LightBuffer lights;
	ver::LightSphere sphere;

	ver::ph::Physics physics;
	ver::ph::Scene scene;
	UT::Player player;
	UT::Level level;
	Camera& cam;
};

