#include "IndexBuffer2.h"
#include "GraphicsThrows.m"
#include "Codex.h"

using namespace ver;

IndexBuffer::IndexBuffer(Graphics& gfx, std::string tag, std::span<const uint16_t> indices)
	:tag(std::move(tag)),
	count(uint32_t(indices.size()))
{
	Initialize(gfx, indices);
}

winrt::IAsyncAction ver::IndexBuffer::InitializeAsync(Graphics& gfx, std::string tag, std::span<const uint16_t> indices)
{
	co_await winrt::resume_background();
	tag = std::move(tag);
	count = uint32_t(indices.size());
	Initialize(gfx, indices);
}

void ver::IndexBuffer::Initialize(Graphics& gfx, std::span<const uint16_t> indices)
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = uint32_t(count * sizeof(decltype(indices)::element_type));
	ibd.StructureByteStride = sizeof(decltype(indices)::element_type);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, pIndexBuffer.put()));
}

void IndexBuffer::Bind(Graphics& gfx)noxnd
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.get(), format, 0u));
}
uint32_t IndexBuffer::GetCount()const noexcept
{
	return count;
}

std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(Graphics& gfx, std::string tag,
	std::span<const uint16_t> indices)
{
	assert(tag != "?");
	return Codex::Resolve<IndexBuffer>(gfx, std::move(tag), indices);
}
std::string IndexBuffer::GenerateUID_(std::string_view tag)
{
	using namespace std::string_literals;
	return typeid(IndexBuffer).name() + "#"s + tag.data();
}
std::string IndexBuffer::GetUID() const noexcept
{
	return GenerateUID_(tag);
}

