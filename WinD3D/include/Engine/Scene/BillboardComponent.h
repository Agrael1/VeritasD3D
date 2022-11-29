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
		void SetColor(Graphics& gfx, DirectX::XMVECTOR color)
		{
			DirectX::XMStoreFloat3A(&buffer.color, color);
			cbuf->Update(gfx, buffer);
		}
		void SetPosition(DirectX::XMVECTOR xposition)
		{
			DirectX::XMStoreFloat3A(&position, xposition);
		}
		virtual DirectX::XMMATRIX GetTransformXM() const noexcept override
		{
			return DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3A(&position));
		}
		void SpawnControlWindow() noexcept;
	private:
		DirectX::XMFLOAT3A position{0,0,0};
		struct cbuffer
		{
			DirectX::XMFLOAT3A color{1,1,1};
		}buffer;
		std::shared_ptr<PixelConstantBuffer<cbuffer>> cbuf;
	};
}