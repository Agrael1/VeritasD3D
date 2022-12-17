#pragma once
#include <Engine/Bindable/Bindable.h>
#include <filesystem>
#include <pplawait.h>

struct ID3D11HullShader;

namespace ver
{
	class HullShader : public Bindable
	{
	public:
		HullShader() = default;
		HullShader(Graphics& gfx, std::filesystem::path path);
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path path);
		void Initialize(Graphics& gfx);
	public:
		void Bind(Graphics& gfx) noxnd override;
		ID3DBlob* GetBytecode() const noexcept;
		static std::shared_ptr<HullShader> Resolve(Graphics& gfx, std::filesystem::path path);
		static concurrency::task<std::shared_ptr<HullShader>> ResolveAsync(Graphics& gfx, std::filesystem::path path);
		static std::string GenerateUID(const std::filesystem::path& path);
		std::string GetUID() const noexcept override;
	protected:
		std::filesystem::path path;
		winrt::com_ptr<ID3D11HullShader> pHullShader;
	};
}
