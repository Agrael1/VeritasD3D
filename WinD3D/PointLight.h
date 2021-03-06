#pragma once
#include <Engine/Graphics.h>
#include <Engine/Architecture/ConstantBuffer.h>
#include "SolidSphere.h"

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow()noexcept;
	void Reset()noexcept;
	void Submit(class FrameCommander& frame) const noxnd;
	void Bind(Graphics& gfx, DirectX::FXMMATRIX view)const noexcept;
private:
	struct PointLightCBuf
	{
		DirectX::XMFLOAT3A pos;
		DirectX::XMFLOAT3A ambient;
		DirectX::XMFLOAT3 diffuse;
		float diffuseIntensity;
		float attConst;
		float attLin;
		float attQuad;
	};
private:
	PointLightCBuf cbData;
	mutable SolidSphere mesh;
	mutable PixelConstantBuffer<PointLightCBuf> cbuf;
};