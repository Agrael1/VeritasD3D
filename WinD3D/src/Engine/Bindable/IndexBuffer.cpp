#include <Engine/Bindable/IndexBuffer.h>
#include <Engine/Bindable/Codex.h>
#include <Shared/Checks.h>
#include <d3d11.h>
#include <format>

using namespace ver;


void ver::IndexBuffer::Initialize(Graphics& gfx, std::span<const uint16_t> indices)
{
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
	ver::check_hresult(GetDevice(gfx)->CreateBuffer(&ibd, &isd, pIndexBuffer.put()));
}
void ver::IndexBuffer::Initialize(Graphics& gfx, std::span<const uint32_t> indices)
{
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
	ver::check_hresult(GetDevice(gfx)->CreateBuffer(&ibd, &isd, pIndexBuffer.put()));
}

void IndexBuffer::Bind(Graphics& gfx)noxnd
{
	Bind(*GetContext(gfx));
}
void IndexBuffer::Bind(ID3D11DeviceContext& context)noxnd
{
	context.IASetIndexBuffer(pIndexBuffer.get(), format, 0u);
	ver::check_context();
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
	return std::format("IndexBuffer#{}", tag);
}


