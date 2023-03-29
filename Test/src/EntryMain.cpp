#include <Test/App.h>

struct apartment_guard
{
	apartment_guard()
	{
		winrt::init_apartment();
	}
	~apartment_guard()
	{
		winrt::uninit_apartment();
	}
};

ver::IAsyncAction MainAsync()
{
	Test::App app(1920, 1080);
	co_await app.InitializeAsync();
}

int main()
{
	apartment_guard guard;
	MainAsync().get();
	return 0;
}