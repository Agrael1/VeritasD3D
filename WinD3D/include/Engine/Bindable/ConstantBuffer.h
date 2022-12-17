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
		void Update(Graphics& gfx, std::span<const std::byte> consts);
		void BindToVS(Graphics& gfx);
		void BindToPS(Graphics& gfx);
		void BindToHS(Graphics& gfx);
		void BindToDS(Graphics& gfx);
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
		void Bind(Graphics& gfx)noxnd override
		{
			ConstantBuffer::BindToVS(gfx);
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBufferT<C>
	{
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		void Bind(Graphics& gfx)noxnd override
		{
			ConstantBuffer::BindToPS(gfx);
		}
	};

	template<typename C>
	class HullConstantBuffer : public ConstantBufferT<C>
	{
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		void Bind(Graphics& gfx)noxnd override
		{
			ConstantBuffer::BindToHS(gfx);
		}
	};

	template<typename C>
	class DomainConstantBuffer : public ConstantBufferT<C>
	{
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		void Bind(Graphics& gfx)noxnd override
		{
			ConstantBuffer::BindToDS(gfx);
		}
	};
}