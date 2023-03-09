#pragma once
#include <Engine/Util/VFileDialog.h>
#include <Game/Window.h>
#include <Game/Camera.h>
#include <Engine/Util/ImGUIManager.h>
#include <Engine/Loading/Model.h>
#include <Engine/Graph/DeferredRenderGraph.h>
#include <Engine/Graph/StereoGraph.h>
#include <Engine/Probes/TestModelProbe.h>
#include <Game/Player.h>
#include <Game/Level.h>
#include <Game/Flag.h>
#include <Audio/Audio.h>
#include <Audio/buffer_stream.h>
#include <PhysX/Foundation.h>
#include <Game/Interaction.h>
#include <Game/Pictures.h>
#include <Game/MediaEngine.h>
#include <Game/Video.h>
#include <Game/Grid.h>
#include <Engine/Scene/Cursor.h>
#include <Engine/Probes/TestModelProbe.h>

namespace UT
{
	class App
	{
		enum class State
		{
			Image,
			Game,
			Video,
			Model
		};
	public:
		App(uint32_t width, uint32_t height);
		~App();
		ver::IAsyncAction InitializeAsync();
	public:
		int Go();
	private:
		void DoFrame(float dt);
		void DoFrameModel(float dt);
		void ProcessInput(float dt);
		void CreateRenderGraph();
		void CreateRenderGraphModel();
		void ResetTransform()
		{
			transform.x = transform.y = transform.z = 0.0f;
		}
		void GameTick();

		void ProcessEvents();
		void OnResize();
		void OnRestyle();
		void OnPlay();
		winrt::fire_and_forget OnLoadAsset();
		ver::IAsyncAction LoadModelAsync(std::filesystem::path path);
		ver::IAsyncAction LoadVideoAsync(std::filesystem::path path);
		ver::IAsyncAction LoadImageAsync(std::filesystem::path path);

		void SetState(State st);
		State GetState() { return cur_st; }
	private:
		std::binary_semaphore lock{ 1 };
		Interaction interaction;

		ImGUIManager imgui;
		Window wnd;
		Graphics gfx;

		//options
		MediaEnginePlayer mp;
		std::optional<ver::Video> vid;
		std::optional<ver::Picture> pic;

		std::unique_ptr<Model> model;
		std::unique_ptr<ver::Skybox> skybox;
		std::optional<ver::Cursor> cur;
		Grid grid;
		ver::Gizmo giz;
		MP probe;

		ver::audio::Audio audio;
		ver::audio::ogg_stream song;
		ver::ph::Physics physics;
		ver::ph::Scene scene;

		std::optional<UT::Player> player;

		UT::Level level;
		UT::Flag flag; //to level

		DirectX::XMFLOAT3 transform{};
		DirectX::XMFLOAT4X4 model_transform
		{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};

		std::optional<ver::rg::StereoGraph> rg;
		bool block = false;
		bool paused = false;


		State cur_st;
		__declspec(property(get=GetState, put=SetState))State state;
	};
}


