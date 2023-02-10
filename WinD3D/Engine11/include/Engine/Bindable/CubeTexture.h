#pragma once
#include <Engine/Bindable/Bindable.h>
#include <pplawait.h>
#include <filesystem>

namespace ver
{
	class OutputOnlyDepthStencil;
	class CubeTexture : public Bindable
	{
	public:
		CubeTexture() = default;
		[[deprecated]] CubeTexture(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0);
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path path, uint32_t slot = 0);
		void Initialize(Graphics& gfx);
	public:
		void Bind(Graphics& gfx) noxnd override;
	protected:
		unsigned int slot = 0u;
		std::filesystem::path path;
		winrt::com_ptr<ID3D11ShaderResourceView> pCubeTextureView;
	};
	class DepthCubeTexture : public Bindable
	{
	public:
		DepthCubeTexture(Graphics& gfx, UINT size, UINT slot = 0);
	public:
		void Bind(Graphics& gfx) noxnd override;
		std::shared_ptr<OutputOnlyDepthStencil> GetDepthBuffer(size_t index) const;
	private:
		unsigned int slot;
	protected:
		winrt::com_ptr<ID3D11ShaderResourceView> pTextureView;
		std::vector<std::shared_ptr<OutputOnlyDepthStencil>> depthBuffers;
	};
}
