#include "App.h"
#include <sstream>
#include <random>
#include <iterator>
#include "Melon.h"
#include "Pyramid.h"
#include "Box.h"
#include "Icosahedron.h"
#include "SkinnedBox.h"
#include "GDIPlusManager.h"
#include "ImGUI\imgui.h"

GDIPlusManager gdipm;

App::App() : wnd(800,600,"VTest"), light(wnd.Gfx())
{
	class Factory
	{
	public:
		Factory(Graphics& gfx)
			:
			gfx(gfx)
		{}
		std::unique_ptr<Drawable> operator()()
		{
			return std::make_unique<Icosahedron>(
				gfx, rng, adist, ddist,
				odist, rdist, bdist
				);
		}
	private:
		Graphics& gfx;
		std::mt19937 rng{ std::random_device{}() };
		std::uniform_real_distribution<float> adist{ 0.0f,PI * 2.0f };
		std::uniform_real_distribution<float> ddist{ 0.0f,PI * 0.5f };
		std::uniform_real_distribution<float> odist{ 0.0f,PI * 0.08f };
		std::uniform_real_distribution<float> rdist{ 6.0f,20.0f };
		std::uniform_real_distribution<float> bdist{ 0.4f,2.0f };
		//std::uniform_int_distribution<int> latdist{ 5,20 };
		//std::uniform_int_distribution<int> longdist{ 10,40 };
		//std::uniform_int_distribution<int> typedist{ 0,4 };
	};

	Factory f(wnd.Gfx());
	drawables.reserve(nDrawables);
	std::generate_n(std::back_inserter(drawables), nDrawables, f);

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
		if (a.first)
		{
			return (int)a.second;
		}
		DoFrame(dt);
	}
}

void App::DoFrame(float dt)
{
	const auto s = dt*speed;

	wnd.Gfx().BeginFrame(0.07f, 0.0f, 0.12f);
	wnd.Gfx().SetCamera(cam.GetViewMatrix());
	light.Bind(wnd.Gfx());

	for (auto& d : drawables)
	{
		d->Update(s);
		d->Draw(wnd.Gfx());
	}
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

