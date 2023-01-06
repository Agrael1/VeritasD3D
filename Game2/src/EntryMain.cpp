#include <Game/App.h>
#include <winrt/base.h>
#include <Shared/Exception.h>


int main()
{
	winrt::init_apartment();

	try
	{
		T::App a;
		a.InitializeAsync(1280, 720).get();
		return a.Go();
	}
	catch (const ver::exception& e)
	{
		MessageBox(nullptr, e.what(), e.type().data(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, "No Details on this one", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}