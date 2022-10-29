#pragma once
#include <Architecture/Drawable.h>
#include <Architecture/BindableCommons.h>

class SolidSphere : public Drawable
{
public:
	SolidSphere(Graphics& gfx, float radius);
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
	std::shared_ptr<PixelConstantBuffer<PSColorConstant>> colorBuffer;
};