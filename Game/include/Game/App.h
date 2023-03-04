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
#include <Engine/Scene/Cursor.h>

namespace UT
{
	class App
	{
		enum class State
		{
			Picture,
			Game,
			Video
		};
	public:
		App(uint32_t width, uint32_t height);
		~App();
		ver::IAsyncAction InitializeAsync();
	public:
		int Go();
	private:
		void InitializeVideo();
		void ShutVideo();

		void DoFrame(float dt);
		void ProcessInput(float dt);
		void CreateRenderGraph();
		void ResetTransform()
		{
			transform.x = transform.y = transform.z = 0.0f;
		}
		void GameTick();
	private:
		Interaction interaction;

		ImGUIManager imgui;
		Window wnd;
		Graphics gfx;
		std::optional<ver::Cursor> cur;
		ver::Gizmo giz;

		ver::audio::Audio audio;
		ver::audio::ogg_stream song;

		MediaEnginePlayer mp;
		std::optional <ver::Video> vid;

		ver::ph::Physics physics;
		ver::ph::Scene scene;
		std::optional<ver::Picture> pic;

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
		State state = State::Game;
	};
}


