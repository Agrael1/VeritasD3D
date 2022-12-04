#pragma once
#include <DirectXMath.h>
#include <filesystem>
#include <Engine/Bindable/ConstantBuffer2.h>
#include <Engine/Rendering/Drawable.h>

namespace ver
{
	class BillboardComponent : public Drawable
	{
		static inline constexpr auto aa = 4;
	public:
		BillboardComponent() = default;
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, std::filesystem::path tex_path, DirectX::XMFLOAT2 dims, bool spherical = true);
	public:
		void SetColor(Graphics& gfx, DirectX::XMVECTOR color)
		{
			DirectX::XMStoreFloat3(&buffer.color, DirectX::XMColorRGBToHSL(color));
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
		void Update(Graphics& gfx);
		void SpawnControlWindow() noexcept;
	private:
		DirectX::XMFLOAT3A position{0,0,0};
		uint32_t frame_count = 1;
		uint32_t speed = 0;
		struct cbuffer
		{
			DirectX::XMFLOAT3 color{1,1,1};
			uint32_t frame = 0u;
		}buffer;
		std::shared_ptr<PixelConstantBuffer<cbuffer>> cbuf;
	};
}