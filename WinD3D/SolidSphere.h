#pragma once
#include <Engine/Architecture/Drawable.h>

class SolidSphere : public Drawable
{
public:
	SolidSphere(Graphics& gfx, float radius, DirectX::XMFLOAT3 color = { 1.0f,1.0f,1.0f });
public:
	void SetPos(DirectX::XMFLOAT3 pos)noexcept;
	void SetColor(DirectX::XMFLOAT3 color)noexcept;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
private:
	DirectX::XMFLOAT3 pos = { 1.0f,1.0f,1.0f };
	DirectX::XMFLOAT3 color;
};