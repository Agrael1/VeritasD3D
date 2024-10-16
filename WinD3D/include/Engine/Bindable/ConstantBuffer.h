#pragma once
#include <Engine/Bindable/Bindable.h>
#include <span>

struct ID3D11Buffer;

namespace ver
{
	class ConstantBuffer : public Bindable
	{
	public:
		ConstantBuffer() = default;
		ConstantBuffer(Graphics& gfx, std::span<const std::byte> consts, uint32_t slot);
		ConstantBuffer(Graphics& gfx, uint32_t size, uint32_t slot);
	public:
		void SetSlot(uint32_t xslot) { slot = xslot; }
		void Update(Graphics& gfx, std::span<const std::byte> consts);
		void BindToVS(ID3D11DeviceContext& context);
		void BindToPS(ID3D11DeviceContext& context);
		void BindToHS(ID3D11DeviceContext& context);
		void BindToDS(ID3D11DeviceContext& context);
	protected:
		winrt::com_ptr<ID3D11Buffer> pConstantBuffer;
		uint32_t slot = 0;
	};

	template<typename C>
	class ConstantBufferT : public ConstantBuffer
	{
	public:
		ConstantBufferT() = default;
		ConstantBufferT(Graphics& gfx, const C& consts, uint32_t slot)
			:ConstantBuffer(gfx, { reinterpret_cast<const std::byte*>(&consts), sizeof(C)}, slot)
		{}
		ConstantBufferT(Graphics& gfx, uint32_t slot)
			:ConstantBuffer(gfx, sizeof(C), slot)
		{}
	public:
		void Update(Graphics& gfx, const C& consts)
		{
			ConstantBuffer::Update(gfx, { reinterpret_cast<const std::byte*>(&consts), sizeof(C) });
		}
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBufferT<C>
	{
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		using Bindable::Bind;
		void Bind(ID3D11DeviceContext& context)noxnd override
		{
			ConstantBuffer::BindToVS(context);
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBufferT<C>
	{
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		using Bindable::Bind;
		void Bind(ID3D11DeviceContext& context)noxnd override
		{
			ConstantBuffer::BindToPS(context);
		}
	};

	template<typename C>
	class HullConstantBuffer : public ConstantBufferT<C>
	{
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		using Bindable::Bind;
		void Bind(ID3D11DeviceContext& context)noxnd override
		{
			ConstantBuffer::BindToHS(context);
		}
	};

	template<typename C>
	class DomainConstantBuffer : public ConstantBufferT<C>
	{
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		using Bindable::Bind;
		void Bind(ID3D11DeviceContext& context)noxnd override
		{
			ConstantBuffer::BindToDS(context);
		}
	};
}