#include <Engine/Bindable/ConstantBuffer2.h>
#include <Engine/Deprecated/GraphicsThrows.h>
#include <Engine/Util/DXGIInfoManager.h>
#include <Engine/Util/GraphicsExceptions.h>

void ver::ConstantBuffer::Initialize(Graphics& gfx, std::span<const std::byte> consts, uint32_t slot)
{
	this->slot = slot;
	INFOMAN(gfx);

	D3D11_BUFFER_DESC cbd{};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = uint32_t(consts.size_bytes());
	cbd.StructureByteStride = 0u;

	D3D11_SUBRESOURCE_DATA csd{};
	csd.pSysMem = consts.data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, pConstantBuffer.put()));
}

void ver::ConstantBuffer::Initialize(Graphics& gfx, uint32_t size, uint32_t slot)
{
	this->slot = slot;
	INFOMAN(gfx);

	D3D11_BUFFER_DESC cbd {};
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.MiscFlags = 0u;
	cbd.ByteWidth = size;
	cbd.StructureByteStride = 0u;

	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, pConstantBuffer.put()));
}

void ver::ConstantBuffer::Update(Graphics& gfx, std::span<const std::byte> consts)
{
	INFOMAN(gfx);

	D3D11_MAPPED_SUBRESOURCE msr{};
	GFX_THROW_INFO(GetContext(gfx)->Map(
		pConstantBuffer.get(), 0u,
		D3D11_MAP_WRITE_DISCARD, 0u,
		&msr
	));
	memcpy(msr.pData, consts.data(), consts.size_bytes());
	GetContext(gfx)->Unmap(pConstantBuffer.get(), 0u);
}

void ver::ConstantBuffer::BindToVS(Graphics& gfx)
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->VSSetConstantBuffers(slot, 1u, (ID3D11Buffer**)&pConstantBuffer));
}

void ver::ConstantBuffer::BindToPS(Graphics& gfx)
{
	INFOMAN_NOHR(gfx);
	GFX_THROW_INFO_ONLY(GetContext(gfx)->PSSetConstantBuffers(slot, 1u, (ID3D11Buffer**)&pConstantBuffer));
}
