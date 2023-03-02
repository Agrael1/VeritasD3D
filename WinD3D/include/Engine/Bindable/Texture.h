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
		Texture(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0, bool test = false);
		ver::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0, bool test = false);
		void Initialize(Graphics& gfx);
	public:
		std::pair<float, float> Dimensions()const noexcept
		{
			return { width, height };
		}
		void Bind(Graphics& gfx) noxnd override;
		void BindTo(Graphics& gfx, uint32_t slot) noxnd;
		static std::shared_ptr<Texture> Resolve(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0, bool test = false);
		static concurrency::task<std::shared_ptr<Texture>> ResolveAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0, bool test = false);
		

		static std::string GenerateUID(const std::filesystem::path& path, uint32_t slot = 0, bool test = false);
		std::string GetUID() const noexcept override;

		bool UsesAlpha() const noexcept { return hasAlpha; }
		uint32_t GetCount()const noexcept { return count; }
		auto& SRV()const noexcept { return pTextureView; }
		auto Resource()const noexcept {
			winrt::com_ptr<ID3D11Resource> res;
			pTextureView->GetResource(res.put());
			return res.as<ID3D11Texture2D>();
		}
	private:
		void ResolveToDefault(Graphics& gfx);
	protected:
		uint32_t slot = 0u;
		uint32_t count = 1u;
		float width = 0.0f;
		float height = 0.0f;

		bool hasAlpha = false;
		bool test = false;
		std::filesystem::path path;
		winrt::com_ptr<ID3D11ShaderResourceView> pTextureView;
	};

	class StagingTexture : public GraphicsResource
	{
	public:
		StagingTexture(Graphics& gfx, DXGI_FORMAT format, uint32_t width, uint32_t height, uint32_t cpu_flags);
	public:
		auto& Resource()const noexcept
		{
			return pTexture;
		}
	private:
		winrt::com_ptr<ID3D11Texture2D> pTexture;
	};
}
