#include <Game/App.h>
#include <imgui.h>
#include <Engine/Util/Utility.h>
#include <Util/Filters.h>

namespace dx = DirectX;

App::App(uint32_t width, uint32_t height)
	: wnd(width, height, "Unreal Tournament"), gfx(wnd.GetHandle(), width, height)
	, player(scene)
	, scene(physics)
{
	gfx.SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(height) / float(width), 0.5f, 1000.0f));
	ResetTransform();
}
App::~App()
{
}

winrt::IAsyncAction App::InitializeAsync()
{
	co_await winrt::when_all(lights.InitializeAsync(gfx),
		sphere.InitializeAsync(lights, gfx),
		level.InitializeAsync(physics, gfx, u"../models/face/face.obj"));
	CreateRenderGraph();
	level.AddToScene(scene);
	co_return;
}

int App::Go()
{
	float dt = 1.0f / 60.0f;
	while (true)
	{
		ResetTransform();
		if (const auto a = wnd.ProcessMessages())
			return (int)a.value();

		scene.get_scene().simulate(dt);
		DoFrame(dt);
		scene.get_scene().fetchResults(true);
		player.Update(transform, dt);
		player.Sync();
	}
}

void App::DoFrame(float dt)
{
	if (!wnd.IsActive())return;

	gfx.BeginFrame(0.2f, 0.2f, 0.2f);
	gfx.SetCamera(player.GetViewMatrix());
	gfx.SetShadowCamPos(DirectX::XMLoadFloat4A(&lights.at(0).pos));
	sphere.Bind(gfx);
	lights.Bind(gfx);

	level.Submit();
	sphere.Submit();
	rg->Execute(gfx);



	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(),
		ImGuiDockNodeFlags_PassthruCentralNode |
		ImGuiDockNodeFlags_NoDockingInCentralNode);

	if (ImGui::Begin("Simulation speed"))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	ProcessInput(dt);
	sphere.SpawnControlWindow();

	// Present
	gfx.EndFrame();
	rg->Reset();
}

void App::ProcessInput(float dt)
{
	while (const auto e = wnd.kbd.ReadKey())
	{
		if (!e->IsPress())
		{
			continue;
		}

		switch (e->GetCode())
		{
		case 'F':
		case VK_INSERT:
			if (wnd.CursorEnabled())
			{
				bFlightMode = true;
				wnd.DisableCursor();
				wnd.mouse.EnableRaw();
			}
			else
			{
				bFlightMode = false;
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
			}
			break;
		case VK_ESCAPE:
			if (bFlightMode)
			{
				bFlightMode = false;
				wnd.EnableCursor();
				wnd.mouse.DisableRaw();
				break;
			}
			PostQuitMessage(0);
			return;
		}
	}

	if (!wnd.CursorEnabled())
	{
		if (wnd.kbd.KeyIsPressed(VK_SHIFT))
		{
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
			//cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('C'))
		{
			//cam.Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			player.Rotate((float)delta->x, (float)delta->y);
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
	sphere.LinkTechniques(*rg);
	level.GetWorld().LinkTechniques(*rg);
}

