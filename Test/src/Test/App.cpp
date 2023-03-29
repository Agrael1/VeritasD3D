#include <Test/App.h>

Test::App::App(uint32_t width, uint32_t height)
	:wnd(width, height, "VTest")
{
}
ver::IAsyncAction Test::App::InitializeAsync()
{
	co_await gfx.InitializeAsync();
	co_await gfx.InitializeSwapChainAsync(
		{
			.width = uint32_t(wnd.GetWidth()),
			.height = uint32_t(wnd.GetHeight())
		},
		ver::SurfaceParameters{
			wnd.GetHandle()
		}
	);

}