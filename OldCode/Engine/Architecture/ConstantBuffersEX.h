#pragma once
#include "Bindable.h"
#include "GraphicsThrows.m"
#include "DynamicConstant.h"
#include <Probes/TechniqueProbe.h>


class ConstantBufferEx : public Bindable
{
public:
	void Update(Graphics& gfx, const DC::Buffer& buf)
	{
		assert(&buf.GetRootLayoutElement() == &GetRootLayoutElement());
		

		D3D11_MAPPED_SUBRESOURCE msr;
		ver::check_hresult(GetContext(gfx)->Map(
			pConstantBuffer.Get(), 0u,
			D3D11_MAP_WRITE_DISCARD, 0u,
			&msr
		));
		memcpy(msr.pData, buf.GetData(), buf.GetSizeInBytes());
		GetContext(gfx)->Unmap(pConstantBuffer.Get(), 0u);
	}
	// this exists for validation of the update buffer layout
	// reason why it's not getbuffer is becasue nocache doesn't store buffer
	virtual const DC::LayoutElement& GetRootLayoutElement() const noexcept = 0;
protected:
	ConstantBufferEx(Graphics& gfx, const DC::LayoutElement& layoutRoot, UINT slot, const DC::Buffer* pBuf)
		:
		slot(slot)
	{
		

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = (UINT)layoutRoot.GetSizeInBytes();
		cbd.StructureByteStride = 0u;

		if (pBuf != nullptr)
		{
			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = pBuf->GetData();
			ver::check_hresult(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &pConstantBuffer));
		}
		else
		{
			ver::check_hresult(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &pConstantBuffer));
		}
	}
protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> pConstantBuffer;
	UINT slot;
};

class PixelConstantBufferEx : public ConstantBufferEx
{
public:
	using ConstantBufferEx::ConstantBufferEx;
	void Bind(Graphics& gfx) noxnd override
	{
		
		ver::check_hresult_ONLY(GetContext(gfx)->PSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
	}
};

class VertexConstantBufferEx : public ConstantBufferEx
{
public:
	using ConstantBufferEx::ConstantBufferEx;
	void Bind(Graphics& gfx) noxnd override
	{
		
		ver::check_hresult_ONLY(GetContext(gfx)->VSSetConstantBuffers(slot, 1u, pConstantBuffer.GetAddressOf()));
	}
};

template<class T>
class CachingConstantBufferEx : public T
{
public:
	CachingConstantBufferEx(Graphics& gfx, const DC::CookedLayout& layout, UINT slot)
		:
		T(gfx, *layout.ShareRoot(), slot, nullptr),
		buf(DC::Buffer(layout))
	{}
	CachingConstantBufferEx(Graphics& gfx, const DC::Buffer& buf, UINT slot)
		:
		T(gfx, buf.GetRootLayoutElement(), slot, &buf),
		buf(buf)
	{}
	const DC::LayoutElement& GetRootLayoutElement() const noexcept override
	{
		return buf.GetRootLayoutElement();
	}
	const DC::Buffer& GetBuffer() const noexcept
	{
		return buf;
	}
	void SetBuffer(const DC::Buffer& buf_in)
	{
		buf.CopyFrom(buf_in);
		dirty = true;
	}
	void Bind(Graphics& gfx) noexcept override
	{
		if (dirty)
		{
			T::Update(gfx, buf);
			dirty = false;
		}
		T::Bind(gfx);
	}
	void Accept(TechniqueProbe& probe) override
	{
		if (probe.VisitBuffer(buf))
		{
			dirty = true;
		}
	}
private:
	bool dirty = false;
	DC::Buffer buf;
};

using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;
