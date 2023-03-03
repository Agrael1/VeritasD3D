#include <Game/App.h>
#include <Game/Loading.h>
#include <imgui.h>
#include <Engine/Util/Utility.h>
#include <Util/Filters.h>
#include <Engine/Pass/FullscreenPass.h>
#include <Engine/Bindable/Texture.h>

using namespace UT;

namespace dx = DirectX;

App::App(uint32_t width, uint32_t height)
	: wnd(width, height, "Unreal Tournament"), gfx(width, height)
	, scene(physics, &interaction)
{
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(height) / float(width), 0.5f, 1000.0f));
	ResetTransform();
	wnd.ChangeToFullScreen();
	wnd.DisableCursor();
	wnd.mouse.EnableRaw();
}
App::~App()
{
}

ver::IAsyncAction App::InitializeAsync()
{
	co_await winrt::when_all(audio.InitializeAsync(), gfx.CreateSwapChain(wnd.GetHandle()));
	co_await UT::Loading::Execute(gfx);
	
	co_await winrt::when_all(
		level.InitializeAsync(physics, gfx, u"../models/face/faceWIP.obj"), 
		song.InitializeAsync(audio, u"../music/foregone.ogg"));

	pic.emplace(gfx);
	pic->InitializeAsync(gfx);
	co_await winrt::when_all(
		pic->LoadFileAsync(gfx, u"../models/a1.dds")
	);

	mp.Initialize(gfx.RawDevice(), gfx.RawContext());
	mp.SetStereo(gfx.StereoEnabled());
	mp.SetSource(L"C:\\Users\\SDIG\\source\\repos\\VeritasD3D\\Game\\models/st2.wmv");

	vid.emplace(gfx);

	auto [w, h] = mp.GetNativeVideoSize();
	//co_await vid->InitializeAsync(gfx, h, w);

	cur.emplace(gfx);

	CreateRenderGraph();
	level.AddToScene(scene);
	player.emplace(scene, level.GetLightBuffer(), gfx);
	player->Teleport({ -183.0f, -36.6f, -34.8f });
}

int App::Go()
{
	float dt = 1.0f / 60.0f;
	//song.play();
	while (true)
	{
		ResetTransform();
		if (const auto a = wnd.ProcessMessages())
			return (int)a.value();

		switch (state)
		{
		case UT::App::State::Picture:
			mp.Stop();
			pic->Execute(gfx, 0);
			ProcessInput(dt);
			break;
		case UT::App::State::Game:
			mp.Stop();
			scene.get_scene().simulate(dt);
			DoFrame(dt);
			scene.get_scene().fetchResults(true);
			player->Update(transform, dt);
			player->Sync();
			GameTick();
			break;
		case UT::App::State::Video:
		{
			//song.pause();

			if(!mp.IsPlaying())
				mp.Play();

			mp.TransferDirect(vid->SRV());;
			
			vid->Execute(gfx);
			ProcessInput(dt);
			break;
		}
		default:
			break;
		}


	}
}
void App::GameTick()
{
	interaction.Apply();
	if(!block)block = level.PollFinish();
	float y = player->GetPosition().y;
	if (y < -300.0f)
		player->Respawn({ -183.0f, -36.6f, -34.8f });
}

void App::DoFrame(float dt)
{
	if (!wnd.IsActive())return;

	gfx.BeginFrame(0.2f, 0.2f, 0.2f);
	gfx.SetLeftCamera(player->GetLeftViewMatrix());
	gfx.SetRightCamera(player->GetRightViewMatrix());
	gfx.SetCamera(player->GetCentralCamera());

	cur->Submit();
	level.Submit(gfx);
	rg->Execute(gfx);

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiDockNodeFlags_NoDockingInCentralNode);

	if (ImGui::Begin("Simulation speed", nullptr, ImGuiWindowFlags_NoDecoration| ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	ProcessInput(dt);
	level.SpawnControlWindow();
	player->SpawnControlWindow();

	// Present
	gfx.EndFrame();
	rg->Reset();
}

void App::ProcessInput(float)
{
	float dt = gfx.GetFrameStep();
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case 'P':
			song.pause();
			state = State::Picture;
			break;
		case 'G':
			song.play();
			state = State::Game;
			break;
		case 'V':
			song.pause();
			state = State::Video;
			break;
		case 'M':
			if (paused^=true)
				song.pause();
			else 
				song.play();
			break;
		case 'F':
			player->ToggleFlight();
			return;
		case VK_INSERT:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_ESCAPE:
			if (wnd.CursorEnabled())
			{
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
				break;
			}
			PostQuitMessage(0);
			return;
		}
	}

	if (wnd.CursorEnabled())
	{
		auto [x, y] = wnd.mouse.GetPos();
		gfx.SetCursor({ short(x), short(y) });
	}

	if (!wnd.CursorEnabled()&&!block)
	{
		gfx.SetCursor({ short(gfx.GetWidth()/2), short(gfx.GetHeight() / 2) });
		if (player->Flight())dt *= 3;
		if (wnd.kbd.KeyIsPressed(VK_SHIFT))
		{
			if(!player->IsMidair()&&!player->Flight())
				dt *= 2;
		}
		if (wnd.kbd.KeyIsPressed('W'))
		{
			transform.z += dt;
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			transform.x -= dt;
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			transform.z -= dt;
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			transform.x += dt;
		}
		if (wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			if (player->Flight())
				transform.y += dt;
			else
				player->Jump();
		}
		if (wnd.kbd.KeyIsPressed('C'))
		{
			if (player->Flight())
				transform.y -= dt;
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			player->Rotate((float)delta->x, (float)delta->y);
		}
	}

	if (wnd.ResizeCalled())
	{
		rg.reset();
		gfx.OnResize(wnd.GetWidth(), wnd.GetHeight());
		CreateRenderGraph();
		gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(wnd.GetHeight()) / float(wnd.GetWidth()), 0.5f, 1000.0f));
		wnd.ResizeComplete();
	}

	if (wnd.RestyleCalled())
	{
		imgui.SetStyle((ImGUIManager::Style(wnd.GetStyle())));
		wnd.RestyleComplete();
	}
}

void App::CreateRenderGraph()
{
	rg.emplace(gfx);
	cur->LinkTechniques(*rg);
	level.Link(*rg);
}

