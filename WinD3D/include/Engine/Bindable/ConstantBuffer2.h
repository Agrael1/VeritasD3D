#pragma once
#include <Engine/Bindable/Codex.h>
#include <span>

struct ID3D11Buffer;

namespace ver
{
	class ConstantBuffer : public Bindable
	{
	public:
		ConstantBuffer() = default;
		ConstantBuffer(Graphics& gfx, std::span<const std::byte> consts, uint32_t slot)
		{
			Initialize(gfx, consts, slot);
		}
		ConstantBuffer(Graphics& gfx, uint32_t size, uint32_t slot)
		{
			Initialize(gfx, size, slot);
		}
		void Initialize(Graphics& gfx, std::span<const std::byte> consts, uint32_t slot);
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, std::span<const std::byte> consts, uint32_t slot)
		{
			co_await winrt::resume_background();
			Initialize(gfx, consts, slot);
		}
		void Initialize(Graphics& gfx, uint32_t size, uint32_t slot);
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, uint32_t size, uint32_t slot)
		{
			co_await winrt::resume_background();
			Initialize(gfx, size, slot);
		}
	public:
		void Update(Graphics& gfx, std::span<const std::byte> consts);
		void BindToVS(Graphics& gfx);
		void BindToPS(Graphics& gfx);
	protected:
		winrt::com_ptr<ID3D11Buffer> pConstantBuffer;
		uint32_t slot;
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

		void Initialize(Graphics& gfx, const C& consts, uint32_t slot)
		{
			ConstantBuffer::Initialize(gfx, { reinterpret_cast<const std::byte*>(&consts), sizeof(C) }, slot);
		}
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, const C& consts, uint32_t slot)
		{
			co_await ConstantBuffer::InitializeAsync(gfx, { reinterpret_cast<const std::byte*>(&consts), sizeof(C) }, slot);
		}
		
		void Initialize(Graphics& gfx, uint32_t slot)
		{
			ConstantBuffer::Initialize(gfx, sizeof(C), slot);
		}
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, uint32_t slot)
		{
			co_await ConstantBuffer::InitializeAsync(gfx, sizeof(C), slot);
		}

	public:
		void Update(Graphics& gfx, const C& consts)
		{
			ConstantBuffer::Update(gfx, { reinterpret_cast<const std::byte*>(&consts), sizeof(C) });
		}
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBufferT<C>
	{
		using ConstantBuffer::slot;
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		void Bind(Graphics& gfx)noxnd override
		{
			ConstantBuffer::BindToVS(gfx);
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, const C& consts, uint32_t slot = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(Graphics& gfx, uint32_t slot = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, slot);
		}
		static std::string GenerateUID(const C&, uint32_t slot)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(uint32_t slot = 0)
		{
			using namespace std::string_literals;
			return typeid(VertexConstantBuffer).name() + "#"s + std::to_string(slot);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBufferT<C>
	{
		using ConstantBuffer::slot;
	public:
		using ConstantBufferT<C>::ConstantBufferT;
		void Bind(Graphics& gfx)noxnd override
		{
			ConstantBuffer::BindToPS(gfx);
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, const C& consts, uint32_t slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(Graphics& gfx, uint32_t slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, slot);
		}
		static std::string GenerateUID(const C&, uint32_t slot)
		{
			return GenerateUID(slot);
		}
		static std::string GenerateUID(uint32_t slot = 0)
		{
			using namespace std::string_literals;
			return typeid(PixelConstantBuffer).name() + "#"s + std::to_string(slot);
		}
		std::string GetUID() const noexcept override
		{
			return GenerateUID(slot);
		}
	};

}