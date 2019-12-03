#include "App.h"
#include <sstream>
#include <random>
#include <iterator>
#include "ImGUI\imgui.h"
#include "GDIPlusManager.h"
#include "Model.h"

GDIPlusManager gdipm;

App::App() : wnd(800,600,"VTest"), light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 40.0f));
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

	if (ImGui::Begin("Simulation speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed, 0.0f, 3.0f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	// Present
	wnd.Gfx().EndFrame();
}

