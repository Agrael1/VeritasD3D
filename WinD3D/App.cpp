#include "App.h"
#include "ImGUI/imgui.h"
#include "Engine/Entities/TestModelProbe.h"


namespace dx = DirectX;

App::App(uint32_t width, uint32_t height) 
	: wnd(width, height,"VTest"), light(wnd.Gfx())
{
	std::array<COMDLG_FILTERSPEC, 1> filterSpecs =
	{
		{L"Wavefront object file",L"*.obj"}
	};
	opener.SetFileTypes(filterSpecs);
	opener.SetDefaultFolder(LR"(C:\Users\aa\Documents\Visual Studio 2019\Projects\WinD3D\WinD3D\Models)");

	model = std::make_unique<Model>(wnd.Gfx(), "Models\\Sponza\\sponza.obj", 1.0f / 20.0f);
	//cube.SetPos({ 4.0f,0.0f,0.0f });
	//cube2.SetPos({ 0.0f,4.0f,0.0f });
	//cube.LinkTechniques(rg);
	//cube2.LinkTechniques(rg);
	light.LinkTechniques(rg);
	model->LinkTechniques(rg);

	wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(height) / float(width), 0.5f, 100.0f));
}
App::~App()
{
}

int App::Go()
{
	float dt = 0.01f;
	while (true)
	{
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
	

	model->Submit();
	light.Submit();
	//cube.Submit();
	//cube2.Submit();
	rg.Execute(wnd.Gfx());


	if (ImGui::Begin("Simulation speed",nullptr, 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();

	
	static MP modelProbe;

	// imgui windows
	modelProbe.SpawnWindow(*model);

	ProcessInput(dt);
	cam.SpawnControlWindow();
	light.SpawnControlWindow();
	//cube.SpawnControlWindow(wnd.Gfx(), "Cube 1");
	//cube2.SpawnControlWindow(wnd.Gfx(), "Cube 2");

	// Present
	wnd.Gfx().EndFrame();
	rg.Reset();
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

	if (wnd.LoadCalled())
	{
		auto wfilename = opener.GetFilePath();
		if (!wfilename.empty())
		{
			model = std::make_unique<Model>(wnd.Gfx(),ToNarrow(wfilename));
			Codex::Trim();
			model->LinkTechniques(rg);
		}
		wnd.LoadingComplete();
	}
}

