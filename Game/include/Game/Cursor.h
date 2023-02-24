#pragma once
#include <Engine/Rendering/Drawable.h>

namespace ver
{
	class Cursor : public Drawable
	{
	public:
		Cursor(Graphics& gfx, float scale = 10.0f);
	public:
		void SpawnControlWindow();
		void SetPosition(DirectX::XMFLOAT3 xposition)noexcept
		{
			position = xposition;
		}
		DirectX::XMMATRIX GetTransformXM() const noexcept override;
	private:
		DirectX::XMFLOAT3 position;
		float scale;
	};
}