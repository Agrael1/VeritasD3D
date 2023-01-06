#include <Game/App.h>

ver::IAsyncAction T::App::InitializeAsync(uint32_t width, uint32_t height)
{
	co_await gfx.InitializeAsync(width, height, false);
}