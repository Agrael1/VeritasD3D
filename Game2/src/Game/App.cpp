#include <Game/App.h>
#include <Shared/Timer.h>


ver::IAsyncAction UT::App::InitializeAsync(uint32_t width, uint32_t height)
{
	ver::scoped_timer t;

	auto gf = gfx.InitializeAsync(width, height, false);
	wnd.Initialize(width, height, "VTest");
	wnd.ChangeToFullScreen();
	co_await gf;


}

int UT::App::Go()
{
	while (true)
	{
		if (const auto a = wnd.ProcessMessages())
			return (int)a.value();
	}
}