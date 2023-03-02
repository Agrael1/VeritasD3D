#pragma once
#include <Engine/Bindable/CursorTransform.h>
#include <Engine/Scene/SolidSphere2.h>

namespace ver
{
	class Cursor : public Drawable
	{
		 inline static constexpr float radius = 0.2f;
	public:
		Cursor(Graphics& gfx);
	public:
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		struct PSColorConstant
		{
			DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f };
			float padding;
		} colorConst;
		std::shared_ptr<ver::PixelConstantBuffer<PSColorConstant>> colorBuffer;
		CursorTransform ctm;
	};
}