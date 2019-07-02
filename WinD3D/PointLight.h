#pragma once
#include "Graphics.h"
#include "ConstantBuffer.h"
#include "SolidSphere.h"

class PointLight
{
public:
	PointLight(Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow()noexcept;
	void Reset()noexcept;
	void Draw(Graphics& gfx)const noexcept(!IS_DEBUG);
	void Bind(Graphics& gfx)const noexcept;
private:
	struct PointLightCBuf
	{
		alignas(16)DirectX::XMFLOAT3 pos;
		alignas(16)DirectX::XMFLOAT3 material;
		alignas(16)DirectX::XMFLOAT3 ambient;
		alignas(16)DirectX::XMFLOAT3 diffuse;
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