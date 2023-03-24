#include <Core/Graphics.h>
#include <Shared/Profile.h>
#include <iostream>

WinD3D12::Graphics::Graphics()
{
	ver::scoped_profiler p;
	ver::DX12Factory f{false};
	for (auto&& a : f.EnumerateAdapters(ver::AdapterPreference::Performance))
	{
		std::wcout << a.GetDesc().description << "\n";
	}
}
