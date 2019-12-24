#pragma once
#include <Engine/Architecture/ConstantBuffer.h>
#include <Engine/Architecture/Drawable.h>
#include <DirectXMath.h>

class TransformCbuf : public Bindable
{
protected:
	struct Transforms
	{
		DirectX::XMMATRIX modelView;
		DirectX::XMMATRIX modelViewProj;
	};
public:
	TransformCbuf(Graphics& gfx, const Drawable& parent, UINT slot = 0u);
	void Bind(Graphics& gfx) noexcept override;
protected:
	void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
	Transforms GetTransforms(Graphics& gfx) noexcept;
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	const Drawable& parent;
};