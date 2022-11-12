#include <Engine/Bindable/IndexBuffer2.h>
#include <Engine/Bindable/Codex.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>
#include <format>

using namespace ver;


void ver::IndexBuffer::Initialize(Graphics& gfx, std::span<const uint16_t> indices)
{
	INFOMAN(gfx);
	format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = uint32_t(indices.size_bytes());
	ibd.StructureByteStride = sizeof(decltype(indices)::element_type);
	D3D11_SUBRESOURCE_DATA isd = {};
	isd.pSysMem = indices.data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &isd, pIndexBuffer.put()));
}
void ver::IndexBuffer::Initialize(Graphics& gfx, std::span<const uint32_t> indices)
{
	INFOMAN(gfx);
	format = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.CPUAccessFlags = 0u;
	ibd.MiscFlags = 0u;
	ibd.ByteWidth = uint32_t(indices.size_bytes());
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

std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(Graphics& gfx, std::string tag,
	std::span<const uint16_t> indices)
{
	assert(tag != "?");
	return Codex::Resolve<IndexBuffer>(gfx, std::move(tag), indices);
}
std::shared_ptr<IndexBuffer> IndexBuffer::Resolve(Graphics& gfx, std::string tag,
	std::span<const uint32_t> indices)
{
	assert(tag != "?");
	return Codex::Resolve<IndexBuffer>(gfx, std::move(tag), indices);
}
std::string IndexBuffer::GenerateUID_(std::string_view tag)
{
	return std::format("{}#{}", typeid(IndexBuffer).name(), tag);
}


