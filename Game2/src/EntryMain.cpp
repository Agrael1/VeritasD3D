#include <Game/App.h>
#include <winrt/base.h>
#include <format>
#include <Shared/Exception.h>

struct Apartment {
	Apartment() { winrt::init_apartment(); }
	~Apartment() { winrt::uninit_apartment(); }
};

int main()
{
	Apartment apa;
	try
	{
		UT::App a;
		a.InitializeAsync(1280, 720).get();
		return a.Go();
	}
	catch (const ver::exception& e)
	{
		MessageBoxA(nullptr, e.what(), e.type().data(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const winrt::hresult_error& e)
	{
		MessageBoxW(nullptr, e.message().c_str(), L"HRESULT Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBoxA(nullptr, "No Details on this one", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}