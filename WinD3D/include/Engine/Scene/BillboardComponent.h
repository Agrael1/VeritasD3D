#pragma once
#include <DirectXMath.h>
#include <filesystem>
#include <Engine/Bindable/ConstantBuffer2.h>
#include <Engine/Rendering/Drawable.h>

namespace ver
{
	class BillboardComponent : public Drawable
	{
	public:
		BillboardComponent() = default;
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path tex_path, DirectX::XMFLOAT2 dims);
	public:
		virtual DirectX::XMMATRIX GetTransformXM() const noexcept override
		{
			return DirectX::XMMatrixIdentity();
		}
	private:
		DirectX::XMFLOAT2 dimensions;
		struct cbuffer
		{
			DirectX::XMFLOAT3 position;
		}buffer;
		std::shared_ptr<VertexConstantBuffer<cbuffer>> cbuf;
	};
}