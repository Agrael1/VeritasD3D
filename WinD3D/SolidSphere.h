#pragma once
#include <Engine/Architecture/Drawable.h>

class SolidSphere : public Drawable
{
public:
	SolidSphere(Graphics& gfx, float radius, DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f });
public:
	void SetPos(DirectX::XMFLOAT3 pos)noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	struct PSColorConstant
	{
		alignas(16)DirectX::XMFLOAT3 color{ 1.0f,1.0f,0.0f };
	}colorConst;
};