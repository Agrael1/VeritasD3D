#pragma once
#include <Engine/Architecture/ConstantBuffer.h>
#include <Engine/Architecture/Drawable.h>
#include <DirectXMath.h>

class TransformCbuf : public CloningBindable
{
protected:
	struct Transforms
	{
		DirectX::XMMATRIX modelView;
		DirectX::XMMATRIX modelViewProj;
	};
public:
	TransformCbuf(Graphics& gfx, UINT slot = 0u);
	void Bind(Graphics& gfx) noxnd override;
	void InitializeParentReference(const Drawable& parent) noexcept override;
	std::unique_ptr<CloningBindable> Clone() const noexcept override;
protected:
	void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
	Transforms GetTransforms(Graphics& gfx) noexcept;
private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	const Drawable* pParent = nullptr;
};