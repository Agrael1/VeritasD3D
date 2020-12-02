#include "App.h"
#include "ImGUI/imgui.h"
#include "Engine/Entities/TestModelProbe.h"


namespace dx = DirectX;

App::App(uint32_t width, uint32_t height) 
	: wnd(width, height,"VTest"), light(wnd.Gfx()), grid(wnd.Gfx())
{
	std::array<COMDLG_FILTERSPEC, 1> filterSpecs =
	{
		{L"Wavefront object file",L"*.obj"}
	};
	opener.SetFileTypes(filterSpecs);
	CreateRenderGraph();
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
	wnd.Gfx().BeginFrame(0.2f, 0.2f, 0.2f);
	wnd.Gfx().SetCamera(cam.GetViewMatrix());
	light.Bind(wnd.Gfx(), cam.GetViewMatrix());
	
	if(model)model->Submit();
	light.Submit();
	if(wnd.DrawGrid())grid.Submit();
	rg->Execute(wnd.Gfx());


	if (ImGui::Begin("Simulation speed",nullptr, 
		ImGuiWindowFlags_::ImGuiWindowFlags_NoInputs))
	{
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
	ImGui::End();


	static MP modelProbe;
	if (model)
		modelProbe.SpawnWindow(*model);	// imgui windows

	ProcessInput(dt);
	cam.SpawnControlWindow();
	light.SpawnControlWindow();

	// Present
	wnd.Gfx().EndFrame();
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
			model->LinkTechniques(*rg);
		}
		wnd.LoadingComplete();
	}

	if (wnd.ResizeCalled())
	{
		grid.UnlinkTechniques();
		light.UnlinkTechniques();
		if (model) model->UnlinkTechniques();
		rg.reset();
		wnd.Gfx().OnResize(wnd.GetWidth(), wnd.GetHeight());
		CreateRenderGraph();
		wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(wnd.GetHeight()) / float(wnd.GetWidth()), 0.5f, 100.0f));
		wnd.ResizeComplete();
	}
}

void App::CreateRenderGraph()
{
	rg.emplace(wnd.Gfx());

	grid.LinkTechniques(*rg);
	light.LinkTechniques(*rg);
	if (model) model->LinkTechniques(*rg);
}

