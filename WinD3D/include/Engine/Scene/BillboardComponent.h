#pragma once
#include <DirectXMath.h>
#include <filesystem>
#include <Engine/Bindable/ConstantBuffer2.h>
#include <Engine/Rendering/Drawable.h>

namespace ver
{
	class BillboardComponent : public Drawable
	{
		friend class BillboardBuilder;
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
		void Draw()const noxnd
		{

		}
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

	class BillboardBuilder final
	{
	public:
		BillboardBuilder() = default;
	public:
		BillboardBuilder& At(DirectX::XMFLOAT3A position) { this->position = position; return*this; }
		BillboardBuilder& Dimensions(DirectX::XMFLOAT2 dims) { this->dims = dims; return*this; }
		BillboardBuilder& Color(DirectX::XMFLOAT3 color) 
		{ 
			DirectX::XMStoreFloat3(&this->color, DirectX::XMColorRGBToHSL(DirectX::XMLoadFloat3(&color)));
			return*this;
		}
		BillboardBuilder& Cylindrical() { this->spherical = false; return*this;}
		BillboardBuilder& Spherical() { this->spherical = true; return*this;}
		BillboardBuilder& Texture(std::filesystem::path p) { this->tex_path = std::move(p); return*this;}

		BillboardComponent Make(Graphics& gfx);
		winrt::IAsyncAction MakeAsync(Graphics& gfx, BillboardComponent& component);
	private:
		DirectX::XMFLOAT3A position{ 0,0,0 };
		DirectX::XMFLOAT3 color{ 1,1,1 };
		DirectX::XMFLOAT2 dims{0,0};
		std::filesystem::path tex_path;
		bool spherical = true;
	};
}