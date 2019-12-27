#include "App.h"
#include <random>
#include <iterator>
#include "ImGUI\imgui.h"
#include "GDIPlusManager.h"
#include "Model.h"

GDIPlusManager gdipm;

App::App() : wnd(1280,720,"VTest"), light(wnd.Gfx()), test(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(720.0f / 1280.0f), 0.5f, 40.0f));
}
App::~App()
{
}

int App::Go()
{
	float dt = 0.005f;
	while (true)
	{
		//dt += 0.05f;
		const auto a = Window::ProcessMessages();
		if (a)
		{
			return (int)a.value();
		}
		DoFrame(dt);
	}
}

void App::DoFrame(float dt)
{
	const auto s = dt*speed;
	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetViewMatrix());
	light.Bind(wnd.Gfx(), cam.GetViewMatrix());
	

	nano.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());
	//test.Draw(wnd.Gfx());

	if (ImGui::Begin("Simulation speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed, 0.0f, 3.0f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
	ProcessInput(dt);
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	//test.SpawnControlWindow(wnd.Gfx());
	nano.ShowWindow("Nanosuit");

	// Present
	wnd.Gfx().EndFrame();
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
			cam.Translate({ 0.0f,0.0f,dt });
		}
		if (wnd.kbd.KeyIsPressed('A'))
		{
			cam.Translate({ -dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed('S'))
		{
			cam.Translate({ 0.0f,0.0f,-dt });
		}
		if (wnd.kbd.KeyIsPressed('D'))
		{
			cam.Translate({ dt,0.0f,0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_SPACE))
		{
			cam.Translate({ 0.0f,dt,0.0f });
		}
		if (wnd.kbd.KeyIsPressed(VK_CONTROL))
		{
			cam.Translate({ 0.0f,-dt,0.0f });
		}
	}

	while (const auto delta = wnd.mouse.ReadRawDelta())
	{
		if (!wnd.CursorEnabled())
		{
			cam.Rotate((float)delta->x, (float)delta->y);
		}
	}
}

