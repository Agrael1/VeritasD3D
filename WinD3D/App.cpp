#include "App.h"
#include <sstream>


App::App() : wnd(800,600,"VTest")
{
}
App::~App()
{
}

int App::Go()
{
	float dt = 0;
	while (true)
	{
		dt += 0.05f;
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
	wnd.Gfx().ClearBuffer(0.5f, 0, 0.5f);
	wnd.Gfx().DrawTestTriangle(dt,
		(float)wnd.mouse.GetAbsolute().x/400.0f - 1.0f,
		-(float)wnd.mouse.GetAbsolute().y/300.0f + 1.0f
		);
	wnd.Gfx().DrawTestTriangle(-dt,
		0,
		0
	);
	wnd.Gfx().EndFrame();
}

