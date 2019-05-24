#include "App.h"
#include <sstream>


App::App() : wnd(800,600,"VTest")
{
	this->bDirectInput = false;
}
App::~App()
{
}

int App::Go()
{
	while (true)
	{
		const auto a = Window::ProcessMessages();
		if (a.first)
		{
			return (int)a.second;
		}
		DoFrame();
	}
}

void App::DoFrame()
{
	wnd.Gfx().ClearBuffer(0.5f, 0, 0.5f);
	wnd.Gfx().DrawTestTriangle();
	wnd.Gfx().EndFrame();
}
