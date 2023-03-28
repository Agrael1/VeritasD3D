#include <Core/Graphics.h>
#include <Shared/Profile.h>
#include <iostream>

WinD3D12::Graphics::Graphics()
{
	ver::scoped_profiler p;
	ver::Factory factory;
	ver::Device device;

	for (auto&& a : factory.EnumerateAdapters(ver::AdapterPreference::Performance))
	{
		std::wcout << a.GetDesc().description << "\n";
		if (a.GetDesc().IsSoftware()) 
			ver::spd_warn("Loading WARP adapter");
		if (device.Initialize(a))break;
	}

}
