#include "IndexBuffer.h"

#include "GraphicsThrows.m"

IndexBuffer::IndexBuffer(Graphics & gfx, const std::vector<unsigned short>& indicies)
	:count((UINT)indicies.size())
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC ibufDesc = {};
	ibufDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibufDesc.Usage = D3D11_USAGE_DEFAULT;
	ibufDesc.CPUAccessFlags = 0u;
	ibufDesc.MiscFlags = 0u;
	ibufDesc.ByteWidth = UINT(count * sizeof(WORD));
	ibufDesc.StructureByteStride = sizeof(WORD);

	D3D11_SUBRESOURCE_DATA isubResData = {};
	isubResData.pSysMem = indicies.data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibufDesc, &isubResData, &pIndexBuffer));
}

void IndexBuffer::Bind(Graphics& gfx)noexcept
{
	GetContext(gfx)->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT::DXGI_FORMAT_R16_UINT, 0u);
}
UINT IndexBuffer::GetCount()const noexcept
{
	return count;
}
