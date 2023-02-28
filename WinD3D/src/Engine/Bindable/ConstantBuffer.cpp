#include <Engine/Bindable/ConstantBuffer.h>
#include <Shared/Checks.h>
#include <d3d11_4.h>

template<std::integral T>
inline constexpr uint32_t round_to16(T size)
{
	return ((size - 1) | 15) + 1;
}

ver::ConstantBuffer::ConstantBuffer(Graphics& gfx, std::span<const std::byte> consts, uint32_t slot)
	:slot(slot)
{
	D3D11_BUFFER_DESC cbd{};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = round_to16(consts.size_bytes());
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd{};
	csd.pSysMem = consts.data();
	ver::check_hresult(GetDevice(gfx)->CreateBuffer(&cbd, &csd, pConstantBuffer.put()));
}


ver::ConstantBuffer::ConstantBuffer(Graphics& gfx, uint32_t size, uint32_t slot)
	:slot(slot)
{
	D3D11_BUFFER_DESC cbd {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = round_to16(size);
	cbd.StructureByteStride = 0u;

	ver::check_hresult(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, pConstantBuffer.put()));
}

void ver::ConstantBuffer::Update(Graphics& gfx, std::span<const std::byte> consts)
{
	D3D11_MAPPED_SUBRESOURCE msr{};
	GetContext(gfx)->Map(
		pConstantBuffer.get(), 0u,
		D3D11_MAP_WRITE_DISCARD, 0u,
		&msr
	);
	memcpy(msr.pData, consts.data(), consts.size_bytes());
	GetContext(gfx)->Unmap(pConstantBuffer.get(), 0u);
	ver::check_context();
}

void ver::ConstantBuffer::BindToVS(Graphics& gfx)
{
	GetContext(gfx)->VSSetConstantBuffers(slot, 1u, (ID3D11Buffer**)&pConstantBuffer);
	ver::check_context();
}

void ver::ConstantBuffer::BindToPS(Graphics& gfx)
{
	GetContext(gfx)->PSSetConstantBuffers(slot, 1u, (ID3D11Buffer**)&pConstantBuffer);
	ver::check_context();
}

void ver::ConstantBuffer::BindToDS(Graphics& gfx)
{
	GetContext(gfx)->DSSetConstantBuffers(slot, 1u, (ID3D11Buffer**)&pConstantBuffer);
	ver::check_context();
}

void ver::ConstantBuffer::BindToHS(Graphics& gfx)
{
	GetContext(gfx)->HSSetConstantBuffers(slot, 1u, (ID3D11Buffer**)&pConstantBuffer);
	ver::check_context();
}
