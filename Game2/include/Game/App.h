#pragma once
#include <Game/Window.h>
#include <Core/Graphics.h>

namespace UT
{
	class App
	{
	public:
		App() = default;
		//App(uint32_t width, uint32_t height);
		//~App();
		ver::IAsyncAction InitializeAsync(uint32_t width, uint32_t height);
	public:
		int Go();
	private:
		//void DoFrame(float dt);
		//void ProcessInput(float dt);
	private:
		ver::Window wnd;
		Core::Graphics gfx;
	};
}