#pragma once
#include <Engine/Util/VFileDialog.h>
#include <Game/Window.h>
#include <Game/Camera.h>
#include <Engine/Util/ImGUIManager.h>
#include <Engine/Loading/Model.h>
#include <Engine/Graph/DeferredRenderGraph.h>
#include <Engine/Probes/TestModelProbe.h>
#include <Engine/Bindable/Light.h>
#include <Engine/Scene/Text.h>
#include <Engine/Scene/BillboardComponent.h>
#include <Game/Player.h>
#include <Game/Level.h>
#include <Game/Flag.h>
#include <Audio/Audio.h>
#include <Audio/buffer_stream.h>
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
	void ResetTransform()
	{
		transform.x = transform.z = 0.0f;
	}
	void GameTick();
private:
	//Camera cam;
	ImGUIManager imgui;
	Window wnd;
	Graphics gfx;

	ver::audio::Audio audio;
	ver::audio::ogg_stream song;

	std::optional<RG::DeferredRenderGraph> rg;
	bool bFlightMode = false;


	ver::ph::Physics physics;
	ver::ph::Scene scene;
	UT::Player player;
	UT::Level level;
	UT::Flag flag;
	DirectX::XMFLOAT3 transform{};

	ver::BillboardComponent test;
};

