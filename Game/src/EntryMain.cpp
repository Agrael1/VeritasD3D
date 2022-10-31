#include <Game/App.h>
#include <winrt/base.h>


int main()
{
	winrt::init_apartment();
	try
	{
		App a{ 1280, 720 };
		a.InitializeAsync().get();
		return a.Go();
	}
	catch (const ver::exception& e)
	{
		MessageBox(nullptr, e.what(), e.type().data(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch(const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr,"No Details on this one", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}