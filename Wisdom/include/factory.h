#pragma once

// Select default API
#ifdef _WIN32
#include <dx12/dx12_factory.h>
//dx12 
namespace ver
{
	using Factory = APIFactory<DX12Factory>;
	using Adapter = APIAdapter<DX12Adapter>;
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
