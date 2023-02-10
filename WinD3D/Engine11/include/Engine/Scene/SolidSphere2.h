#pragma once
#include <Engine/Rendering/Drawable.h>

namespace ver
{
	template<class C>
	class PixelConstantBuffer;

	class alignas(16) SolidSphere : public Drawable
	{
	public:
		SolidSphere() = default;
		SolidSphere(Graphics& gfx, float radius);
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, float radius);
	public:
		void SetPos(DirectX::XMFLOAT3 pos)noexcept;
		void SetColor(DirectX::XMFLOAT3 color)noexcept;
		void UpdateColor(Graphics& gfx)noexcept;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		std::shared_ptr<ver::PixelConstantBuffer<PSColorConstant>> colorBuffer;
	};
}
