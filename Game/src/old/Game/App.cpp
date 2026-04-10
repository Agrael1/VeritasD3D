#include <Game/App.h>
#include <Game/Loading.h>
#include <imgui.h>
#include <Engine/Util/Utility.h>
#include <Util/Filters.h>
#include <Engine/Pass/FullscreenPass.h>
#include <Engine/Bindable/Texture.h>
#include <Engine/Util/VFileDialog.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/Scene.h>

using namespace UT;


constexpr COMDLG_FILTERSPEC rgSpec[] =
{
	{ L"All Files", L"*.*"},
	{ L"Image Array", L"*.dds"},
	{ L"Videos", L"*.wmv"},
	{ L"Models", L"*.obj"},
};

namespace dx = DirectX;

App::App(uint32_t width, uint32_t height)
	: wnd(width, height, "Unreal Tournament"), gfx(width, height)
	, scene(physics, &interaction)
{
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(height) / float(width), 0.5f, 1000.0f));
	ResetTransform();
	//wnd.ChangeToFullScreen();
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

	mp.Initialize(gfx.RawDevice(), gfx.RawContext());
	mp.SetStereo(gfx.StereoEnabled());
	grid.Initialize(gfx);

	cur.emplace(gfx);
	level.AddToScene(scene);
	player.emplace(scene, level.GetLightBuffer(), gfx);
	player->Teleport({ -183.0f, -36.6f, -34.8f });
	state = State::Game;
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
		ProcessEvents();

		ver::scoped_semaphore xlk{lock};

		switch (state)
		{
		case UT::App::State::Image:
			pic->Execute(gfx);
			ProcessInput(dt);
			break;
		case UT::App::State::Model:
			scene.get_scene().simulate(dt);
			DoFrameModel(dt);
			scene.get_scene().fetchResults(true);
			player->Update(transform, dt);
			player->Sync();
			GameTick();
			break;
		case UT::App::State::Game:
			scene.get_scene().simulate(dt);
			DoFrame(dt);
			scene.get_scene().fetchResults(true);
			player->Update(transform, dt);
			player->Sync();
			GameTick();
			break;
		case UT::App::State::Video:
			if (!mp.IsPlaying())
				mp.Play();
			mp.TransferDirect(vid->SRV());
			vid->Execute(gfx);
			ProcessInput(dt);
			break;
		default:
			break;
		}


	}
}
void App::GameTick()
{
	interaction.Apply();
	if (!block)block = level.PollFinish();
	float y = player->GetPosition().y;
	if (y < -300.0f)
		player->Respawn({ -183.0f, -36.6f, -34.8f });
}

void UT::App::ProcessEvents()
{
	using enum ::Event;
	for (auto e : wnd.GetEvents())
	{
		switch (e)
		{
		case Resize:OnResize(); break;
		case Restyle:OnRestyle(); break;
		case LoadAsset:OnLoadAsset(); break;
		case Play:OnPlay(); break;
		default:break;
		}
	}
}
void UT::App::OnResize()
{
	gfx.OnResize(wnd.GetWidth(), wnd.GetHeight());
	if (state == State::Model)
	{
		CreateRenderGraphModel();
	}
	else
	{
		CreateRenderGraph();
	}
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(wnd.GetHeight()) / float(wnd.GetWidth()), 0.5f, 1000.0f));
	wnd.mouse.BoundCursor(wnd.GetWidth(), wnd.GetHeight());
	gfx.SetCursor(wnd.mouse.GetPos());
}
void UT::App::OnRestyle()
{
	imgui.SetStyle(wnd.GetStyle());
}
void UT::App::OnPlay()
{
	ver::scoped_semaphore xlk{lock};
	CreateRenderGraph();
	player->ToggleFlight();
	player->GetCamera().SetFocus(100.0f);
	player->Teleport({ -183.0f, -36.6f, -34.8f });
	if (wnd.CursorEnabled())
	{
		wnd.DisableCursor();
		wnd.mouse.EnableRaw();
	}
	state = State::Game;
}
winrt::fire_and_forget UT::App::OnLoadAsset()
{
	co_await winrt::resume_background();
	ver::std_info("Loading Issued");
	ver::VFileOpenDialog pick;
	pick.SetFileTypes(rgSpec);
	auto path = pick.GetFilePath();
	if (path.empty()) { ver::std_info("Operation Cancelled."); co_return; }

	auto ext = path.extension();
	if (ext.native().contains(L"wmv"))
		co_await LoadVideoAsync(std::move(path));
	else if (ext.native().contains(L"dds"))
		co_await LoadImageAsync(std::move(path));
	else
		co_await LoadModelAsync(std::move(path));
	wnd.EnableLoading();
}

ver::IAsyncAction UT::App::LoadModelAsync(std::filesystem::path path)
{
	co_await winrt::resume_background();
	ver::std_info("Loading Model");
	auto m = std::make_unique<Model>();
	auto skb = std::make_unique<ver::Skybox>();

	co_await winrt::when_all(
		m->InitializeAsync(gfx, std::move(path), 1.0f),
		skb->InitializeAsync(gfx, u"../models/skybox.dds", false)
	);
	{
		ver::scoped_semaphore xlk{lock};
		std::swap(m, model);
		std::swap(skb, skybox);

		CreateRenderGraphModel();
		player->Teleport({ 0, 10, 0 });
		player->SetFlight(true);

		state = State::Model;
	}
	ver::std_info("Loading Finished");
}
ver::IAsyncAction UT::App::LoadVideoAsync(std::filesystem::path path)
{
	co_await winrt::resume_background();
	ver::std_info("Loading Video");

	mp.SetSource(path.wstring());
	vid.emplace(gfx);
	auto [w, h] = mp.GetNativeVideoSize();
	co_await vid->InitializeAsync(gfx, h, w);

	state = State::Video;
}
ver::IAsyncAction UT::App::LoadImageAsync(std::filesystem::path path)
{
	co_await winrt::resume_background();
	ver::std_info("Loading Image");
	if (!pic)
	{
		pic.emplace(gfx);
		co_await pic->InitializeAsync(gfx);
	}
	co_await pic->LoadFileAsync(gfx, std::move(path));
	state = State::Image;
}

void UT::App::SetState(State st)
{
	if (cur_st == st)return;
	if (cur_st == State::Video)
	{
		mp.Stop();
	}
	if (cur_st == State::Game)
	{
		song.pause();
	}
	if (st == State::Game)
	{
		//song.play();
	}
	cur_st = st;
}

void App::DoFrame(float dt)
{
	using namespace DirectX;
	if (!wnd.IsActive())return;

	gfx.BeginFrame(0.2f, 0.2f, 0.2f);

	gfx.SetLeftCamera(player->GetLeftViewMatrix());
	gfx.SetRightCamera(player->GetRightViewMatrix());
	gfx.SetCentralCamera(player->GetCentralCamera());
	auto ff = player->GetCamera().GetFocus();
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(wnd.GetHeight()) / float(wnd.GetWidth()), ff, 1000.0f+ff));

	cur->Submit();
	level.Submit(gfx);
	rg->Execute(gfx);

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiDockNodeFlags_NoDockingInCentralNode);

	if (ImGui::Begin("Simulation speed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground))
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

void UT::App::DoFrameModel(float dt)
{
	using namespace DirectX;
	if (!wnd.IsActive())return;

	gfx.BeginFrame(0.4f, 0.4f, 0.4f);

	gfx.SetLeftCamera(player->GetLeftViewMatrix());
	gfx.SetRightCamera(player->GetRightViewMatrix());
	gfx.SetCentralCamera(player->GetCentralCamera());

	cur->Submit();
	model->Submit();
	skybox->Submit();
	if (wnd.DrawGrid())
		grid.Submit();

	rg->Execute(gfx);

	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiDockNodeFlags_NoDockingInCentralNode);

	if (ImGui::Begin("Simulation speed", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_::ImGuiWindowFlags_NoBackground))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	ProcessInput(dt);

	probe.SpawnWindow(*model);



	player->SpawnControlWindow();

	if (wnd.CursorEnabled())
	{
		giz.Render(gfx);

		auto& cam = player->GetCamera();
		auto p = cam.GetPosition();
		cam.SetFocus(DirectX::XMVector3LengthEst(giz.GetPosition() - DirectX::XMLoadFloat3(&p)).m128_f32[0]);

		model->SetRootTransform(DirectX::XMLoadFloat4x4(&model_transform) * giz.GetTransformXM());
	}

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
		case 'R':
			DirectX::XMStoreFloat4x4(&model_transform, DirectX::XMMatrixIdentity());
			break;
		case 'M':
			if (paused ^= true)
				song.pause();
			else
				song.play();
			break;
		case 'F':
			player->ToggleFlight();
			break;
		case VK_INSERT:
			if (wnd.CursorEnabled())
			{
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				wnd.EnableCursor();
				//wnd.HideCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case 'J':
			if (!bcur)
				wnd.ShowCursor();
			else
				wnd.HideCursor();
			bcur ^= true;
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
		gfx.SetCursor(wnd.mouse.GetPos());

		while (const auto e = wnd.mouse.Read())
		{
			if (e->LeftIsPressed())
			{
				DirectX::XMStoreFloat4x4(&model_transform,
					DirectX::XMLoadFloat4x4(&model_transform)*giz.GetTransformXM());
				giz.SetPosition(cur->GetTransformXM().r[3]);
			}
		}
	}

	if (!wnd.CursorEnabled() && !block)
	{
		gfx.SetCursor({ short(gfx.GetWidth() / 2), short(gfx.GetHeight() / 2) });
		if (player->Flight())dt *= 3;
		if (wnd.kbd.KeyIsPressed(VK_SHIFT))
		{
			if (!player->IsMidair() && !player->Flight())
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

	wnd.kbd.Flush();
	wnd.mouse.Flush();
}

void App::CreateRenderGraph()
{
	rg.emplace(gfx);
	cur->LinkTechniques(*rg);
	level.Link(*rg);
}

void UT::App::CreateRenderGraphModel()
{
	rg.emplace(gfx);
	cur->LinkTechniques(*rg);
	model->LinkTechniques(*rg);
	skybox->LinkTechniques(*rg);
	grid.LinkTechniques(*rg);
}

