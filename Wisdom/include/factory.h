#pragma once

// Select default API
#ifdef _WIN32
#include <dx12/dx12_factory.h>
#include <dx12/dx12_device.h>

//dx12 
namespace ver
{
	using Factory = DX12Factory;
	using Adapter = DX12Adapter;
	using Device = DX12Device;
	using CommandQueue = DX12CommandQueue;
}
#elif defined(MAC)  //MAC
//metal
namespace ver
{
}
#elif defined(__EMSCRIPTEN__)  //emscripten
//webgpu
namespace ver
{
}
#else
//vulcan
namespace ver
{
}
#endif
