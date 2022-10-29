#pragma once
#include "Architecture/Drawable.h"
#include "Architecture/ConstantBuffer.h"

namespace ver
{
	class Text : public Drawable
	{
	public:
		Text(Graphics& gfx, float radius);
	public:
		void SetPos(DirectX::XMFLOAT3 pos) noexcept;
		void SetColor(DirectX::XMFLOAT3 color) noexcept;
		void UpdateColor(Graphics& gfx) noexcept;
		DirectX::XMMATRIX GetTransformXM() const noexcept;
	private:
		DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		std::shared_ptr<PixelConstantBuffer<PSColorConstant>> colorBuffer;
	};
}