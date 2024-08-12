#include <Game/App.h>
#include <winrt/base.h>


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

int main()
{
	apartment_guard guard;

	try
	{
		arka::App a{ 600, 800 };
		a.InitializeAsync().get();
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