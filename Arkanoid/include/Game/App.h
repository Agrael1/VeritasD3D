#pragma once
//#include <Engine/Util/VFileDialog.h>
#include <Engine/Graph/GalaxyRenderGraph.h>
#include <Game/Window.h>
#include <Game/Camera.h>
#include <Game/Entities/Ball.h>
#include <Game/Entities/Field.h>
#include <Game/Interaction.h>
#include <PhysX/Scene.h>

// 
//#include <Engine/Util/ImGUIManager.h>
//#include <Engine/Loading/Model.h>
//#include <Engine/Graph/StereoGraph.h>
//#include <Engine/Probes/TestModelProbe.h>
//#include <Game/Player.h>
//#include <Game/Level.h>
//#include <Game/Flag.h>
//#include <Audio/Audio.h>
//#include <Audio/buffer_stream.h>
//#include <Game/Pictures.h>
//#include <Game/MediaEngine.h>
//#include <Game/Video.h>
//#include <Game/Grid.h>
//#include <Engine/Scene/Cursor.h>
//#include <Engine/Probes/TestModelProbe.h>

namespace arka
{
	class App
	{
	public:
		App(uint32_t width, uint32_t height);
		~App();
		ver::IAsyncAction InitializeAsync()noexcept;
	public:
		int Go();
	private:
		void DoFrame(float dt);
		void ProcessInput(float dt);
		void CreateRenderGraph();
		void ProcessPhysics();

		void ProcessEvents();
		void OnResize();
		void OnRestyle();

	private:
		Interaction interaction;

		ImGUIManager imgui;

        Window wnd;	   // window
        Graphics gfx;  // graphics engine
        Camera camera; // scene camera

		std::optional<RG::GalaxyRenderGraph> rg;
		ver::ph::Physics physics;
		ver::ph::Scene scene;

        Ball ball;
        Field field;

		//std::optional<ver::Cursor> cur;
		//MP probe;

		//ver::audio::Audio audio;
		//ver::audio::ogg_stream song;


		//std::optional<UT::Player> player;

		//UT::Level level;
		//UT::Flag flag; //to level

		//DirectX::XMFLOAT3 transform{};

		//bool block = false;
		//bool paused = false;
		//bool bcur = false;
		//bool perfect_cam = false;
	};
}


