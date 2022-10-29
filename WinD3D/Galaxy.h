#pragma once
#include <Framework/ImGUIManager.h>
#include <Window.h>
#include <Scene/Camera.h>
#include <Scene/Grid.h>
#include <Scene/SolidSphere.h>
#include <Architecture/GalaxyRenderGraph.h>
#include "../ImGUI/imgui.h"

#include <Datastars.h>


class Galaxy
{
public:
	Galaxy(uint32_t width, uint32_t height)
		:wnd(width, height, "Galaxy"), grid(wnd.Gfx()), ds(wnd.Gfx())
	{
		CreateRenderGraph();
		wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(height) / float(width), 0.5f, 100.0f));
	}
public:
	int Go()
	{
		float dt = 0.01f;
		while (true)
		{
			const auto a = wnd.ProcessMessages();
			if (a)
			{
				return (int)a.value();
			}
			DoFrame(dt);
		}
	}
private:
	void DoFrame(float dt);
	void ProcessInput(float dt)
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
			if (wnd.kbd.KeyIsPressed('C'))
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

		if (wnd.ResizeCalled())
		{
			grid.UnlinkTechniques();
			ds.UnlinkTechniques();

			rg.reset();
			wnd.Gfx().OnResize(wnd.GetWidth(), wnd.GetHeight());
			CreateRenderGraph();
			wnd.Gfx().SetProjection(DirectX::XMMatrixPerspectiveLH(1.0f, float(wnd.GetHeight()) / float(wnd.GetWidth()), 0.5f, 100.0f));
			wnd.ResizeComplete();
		}

		if (wnd.RestyleCalled())
		{
			imgui.SetStyle((ImGUIManager::Style(wnd.GetStyle())));
			wnd.RestyleComplete();
		}
	}
	void CreateRenderGraph()
	{
		rg.emplace(wnd.Gfx());

		grid.LinkTechniques(*rg);
		ds.LinkTechniques(*rg);
	}
private:
	ImGUIManager imgui;
	Window wnd;
	Camera cam;
	Grid grid;
	std::optional<RG::GalaxyRenderGraph> rg;
	
	Datastars ds;

	float speed = 1.0f;
	bool bFlightMode = false;
};
