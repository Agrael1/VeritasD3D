#pragma once
#include "Bindable.h"
#include "GraphicsThrows.m"
#include <Engine/Architecture/VertexLayout.h>

class VertexBuffer : public Bindable
{
public:
	template <class V>
	VertexBuffer(Graphics& gfx, const std::vector<V> Verticies)
		:Stride(sizeof(V))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.CPUAccessFlags = 0u;
		bufDesc.MiscFlags = 0u;
		bufDesc.ByteWidth = UINT(Stride * Verticies.size());
		bufDesc.StructureByteStride = Stride;

		D3D11_SUBRESOURCE_DATA subResData = {};
		subResData.pSysMem = Verticies.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufDesc, &subResData, &pVertexBuffer));
	}
	VertexBuffer(Graphics& gfx, const DV::VertexBuffer& Verticies)
		:Stride(UINT(Verticies.GetLayout().Size()))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC bufDesc = {};
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.CPUAccessFlags = 0u;
		bufDesc.MiscFlags = 0u;
		bufDesc.ByteWidth = UINT(Verticies.Size());
		bufDesc.StructureByteStride = Stride;

		D3D11_SUBRESOURCE_DATA subResData = {};
		subResData.pSysMem = Verticies.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&bufDesc, &subResData, &pVertexBuffer));
	}

	void Bind(Graphics& gfx) noexcept override;

protected:
	UINT Stride;
	Microsoft::WRL::ComPtr<ID3D11Buffer> pVertexBuffer;
};