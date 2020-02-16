#include "VertexLayout.h"

#if _DEBUG

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

DV::Vertex DV::VertexBuffer::Back() noexcept(!IS_DEBUG)
{
	assert(!buffer.empty());
	return Vertex{ buffer.data() + buffer.size() - layout.Size(), layout };
}
DV::Vertex DV::VertexBuffer::Front() noexcept(!IS_DEBUG)
{
	assert(buffer.empty());
	return Vertex{ buffer.data(), layout };
}
DV::Vertex DV::VertexBuffer::operator[](size_t i)noxnd
{
	assert(i < Count());
	return Vertex{ buffer.data() + layout.Size() * i,layout };
}
