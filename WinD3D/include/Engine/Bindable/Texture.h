#pragma once
#include <Engine/Bindable/Bindable.h>
#include <pplawait.h>
#include <filesystem>

namespace ver
{
	class Texture : public Bindable
	{
	public:
		Texture() = default;
		Texture(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0);
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0);
		void Initialize(Graphics& gfx);
	public:
		void Bind(Graphics& gfx) noxnd override;
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0);
		static concurrency::task<std::shared_ptr<Texture>> ResolveAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0);

		static std::string GenerateUID(const std::filesystem::path& path, uint32_t slot = 0);
		std::string GetUID() const noexcept override;

		bool UsesAlpha() const noexcept { return hasAlpha; }
		uint32_t GetCount()const noexcept { return count; }
	private:
		void ResolveToDefault(Graphics& gfx);
	protected:
		uint32_t slot = 0u;
		uint32_t count = 1u;
		bool hasAlpha = false;
		std::filesystem::path path;
		winrt::com_ptr<ID3D11ShaderResourceView> pTextureView;
	};
}
