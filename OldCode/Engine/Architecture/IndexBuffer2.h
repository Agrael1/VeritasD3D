#pragma once
#include <Engine/Bindable/Bindable.h>
#include <span>

namespace ver
{
	class IndexBuffer : public Bindable
	{
	public:
		IndexBuffer() = default;
		IndexBuffer(Graphics& gfx, std::span<const uint16_t> indices)
			:IndexBuffer(gfx, "?", indices){}
		IndexBuffer(Graphics& gfx, std::string tag, std::span<const uint16_t> indices);

		ver::IAsyncAction InitializeAsync(Graphics& gfx, std::string tag, std::span<const uint16_t> indices);
		void Initialize(Graphics& gfx, std::span<const uint16_t> indices);
	public:
		void Bind(Graphics& gfx) noxnd override;
		uint32_t GetCount() const noexcept;

		static std::shared_ptr<IndexBuffer> Resolve(Graphics& gfx, std::string tag,
			std::span<const uint16_t> indices);

		template<typename...Ignore>
		static std::string GenerateUID(std::string_view tag, Ignore&&...ignore)
		{
			return GenerateUID_(tag);
		}
		std::string GetUID() const noexcept override;
	private:
		static std::string GenerateUID_(std::string_view tag);
	protected:
		std::string tag;
		uint32_t count = 0;
		DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_R16_UINT;
		winrt::com_ptr<ID3D11Buffer> pIndexBuffer;
	};
}
