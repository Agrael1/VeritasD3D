#include "App.h"
#include "ImGUI/imgui.h"
#include <Engine/Architecture/DynamicConstant.h>

App::App() : wnd(1280,720,"VTest"), light(wnd.Gfx())
{
	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(720.0f / 1280.0f), 0.5f, 100.0f));

	DC::RawLayout s;
	s.Add<DC::Type::Struct>("butts");
	s["butts"].Add(
		{
			{ DC::Type::Float, "pubes", 6},
			{ DC::Type::Float, "dank"}
		}
	);

	using namespace std::string_literals;
	auto b = DC::Buffer(std::move(s));
	const auto sig = b.GetRootLayoutElement().GetSignature();

	b["butts"s]["dank"s] = 0.2f;
	b["butts"s]["pubes"s][1] = 0.2f;

	//s["butts"].Add<DC::Type::Float3>("pubes");
	//s["butts"].Add<DC::Type::Float>("dank");
	//s.Add<DC::Type::Float>("woot");
	//s.Add<DC::Type::Array>("arr");
	//s["arr"].Set<DC::Type::Struct>(4);
	//s["arr"].T().Add<DC::Type::Float3>("twerk");
	//s["arr"].T().Add<DC::Type::Array>("werk");
	//s["arr"].T()["werk"].Set<DC::Type::Float>(6);
	//s["arr"].T().Add<DC::Type::Array>("meta");
	//s["arr"].T()["meta"].Set<DC::Type::Array>(6);
	//s["arr"].T()["meta"].T().Set<DC::Type::Matrix>(4);
	//s["arr"].T().Add<DC::Type::Bool>("booler");
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
	

	Sponza.Draw(wnd.Gfx());
	light.Draw(wnd.Gfx());

	if (ImGui::Begin("Simulation speed"))
	{
		ImGui::SliderFloat("Speed Factor", &speed, 0.0f, 3.0f);
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();
	ProcessInput(dt);
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	Sponza.ShowWindow("Sponza");

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

