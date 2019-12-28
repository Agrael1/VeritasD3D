#if _DEBUG
#include <Engine/Architecture/VertexLayout.h>

std::array<std::wstring, size_t(DV::Type::Count)> DV::VertexLayout::Typenames
{
	L"Position2D",
	L"Position3D",
	L"Texture2D",
	L"Normal",
	L"Tangent",
	L"Bitangent",
	L"Float3Color",
	L"Float4Color",
	L"BGRAColor"
};

#endif