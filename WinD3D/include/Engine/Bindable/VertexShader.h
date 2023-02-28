#pragma once
#include <Engine/Bindable/Bindable.h>
#include <filesystem>
#include <pplawait.h>

namespace ver
{
	class VertexShader : public Bindable
	{
		static inline constexpr auto shader_folder = L"..\\Shaders\\";
	public:
		VertexShader() = default;
		VertexShader(Graphics& gfx, std::filesystem::path path);
		ver::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path path);
		void Initialize(Graphics& gfx);
	public:
		void Bind(Graphics& gfx) noxnd override;
		void Bind(ID3D11DeviceContext& context) noxnd override;
		ID3DBlob* GetBytecode() const noexcept;
		static std::shared_ptr<VertexShader> Resolve(Graphics& gfx, std::filesystem::path path);
		static concurrency::task<std::shared_ptr<VertexShader>> ResolveAsync(Graphics& gfx, std::filesystem::path path);
		static std::string GenerateUID(const std::filesystem::path& path);
		std::string GetUID() const noexcept override;
	protected:
		std::filesystem::path path;
		winrt::com_ptr<ID3DBlob> pBytecodeBlob;
		winrt::com_ptr<ID3D11VertexShader> pVertexShader;
	};

}
