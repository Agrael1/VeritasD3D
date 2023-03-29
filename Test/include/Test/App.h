#pragma once
#include <Test/Window.h>
#include <Core/Graphics.h>

namespace Test
{
	class App
	{
	public:
		App(uint32_t width, uint32_t height);
	public:
		ver::IAsyncAction InitializeAsync();
	private:
		Window wnd;
		WinD3D12::Graphics gfx;
	};
}