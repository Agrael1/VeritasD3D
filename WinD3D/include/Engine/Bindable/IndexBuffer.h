#pragma once
#include <Engine/Bindable/Bindable.h>
#include <span>
#include <dxgiformat.h>

struct ID3D11Buffer;

namespace ver
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer() = default;
		IndexBuffer(Graphics& gfx, std::span<const uint16_t> indices)
			:IndexBuffer(gfx, "?", indices){}
		IndexBuffer(Graphics& gfx, std::span<const uint32_t> indices)
			:IndexBuffer(gfx, "?", indices){}
		IndexBuffer(Graphics& gfx, std::string tag, std::span<const uint16_t> indices)
			:tag(std::move(tag)),
			count(uint32_t(indices.size()))
		{
			Initialize(gfx, indices);
		}
		IndexBuffer(Graphics& gfx, std::string tag, std::span<const uint32_t> indices)
			:tag(std::move(tag)),
			count(uint32_t(indices.size()))
		{
			Initialize(gfx, indices);
		}

		ver::IAsyncAction InitializeAsync(Graphics& gfx, std::string tag, std::span<const uint16_t> indices)
		{
			co_await winrt::resume_background();
			tag = std::move(tag);
			count = uint32_t(indices.size());
			Initialize(gfx, indices);
		}
		ver::IAsyncAction InitializeAsync(Graphics& gfx, std::string tag, std::span<const uint32_t> indices)
		{
			co_await winrt::resume_background();
			tag = std::move(tag);
			count = uint32_t(indices.size());
			Initialize(gfx, indices);
		}
	private:
		void Initialize(Graphics& gfx, std::span<const uint16_t> indices);
		void Initialize(Graphics& gfx, std::span<const uint32_t> indices);
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd override;
		uint32_t GetCount() const noexcept { return count; }

		static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, std::string tag,
			std::span<const uint16_t> indices);
		static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, std::string tag,
			std::span<const uint32_t> indices);

		template<typename...Ignore>
		static std::string GenerateUID(std::string_view tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID() const noexcept
		{
			return GenerateUID_(tag);
		}
	private:
		static std::string GenerateUID_(std::string_view tag);
	protected:
		std::string tag;
		uint32_t count = 0;
		DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
		winrt::com_ptr<ID3D11Buffer> pIndexBuffer;
	};
}
