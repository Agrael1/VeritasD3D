#pragma once
#include <Engine/Rendering/Drawable.h>

namespace ver
{
	template<class C>
	class PixelConstantBuffer;

	class alignas(16) SolidCube : public Drawable
	{
	public:
		SolidCube() = default;
		SolidCube(Graphics& gfx, float width, float height);
		ver::IAsyncAction InitializeAsync(Graphics& gfx, float width, float height);
	public:
		void SetPos(DirectX::XMFLOAT3 pos)noexcept;
		DirectX::XMFLOAT3 GetPos()const noexcept { return pos; }
		void SetColor(DirectX::XMFLOAT3 color)noexcept;
		void UpdateColor(Graphics& gfx)noexcept;
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		DirectX::XMFLOAT3 pos = { 0.0f,0.0f,0.0f };
		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		std::shared_ptr<ver::PixelConstantBuffer<PSColorConstant>> colorBuffer;
	};
}
