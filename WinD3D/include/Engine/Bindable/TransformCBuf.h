#pragma once
#include <Engine/Bindable/ConstantBuffer.h>
#include <Engine/Rendering/Drawable.h>
#include <DirectXMath.h>
//#include "ConstantBuffer2.h"

class TransformCbuf : public CloningBindable
{
protected:
	struct Transforms
	{
		DirectX::XMMATRIX model;
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

class SkyboxTransformCbuf : public Bindable
{
protected:
	struct Transforms
	{
		DirectX::XMMATRIX viewProj;
		DirectX::XMMATRIX rot;
	};
public:
	SkyboxTransformCbuf(Graphics& gfx, UINT slot = 0u);
	void Bind(Graphics& gfx) noxnd override;
protected:
	void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd;
	Transforms GetTransforms(Graphics& gfx) noxnd;
private:
	std::unique_ptr<VertexConstantBuffer<Transforms>> pVcbuf;
	float f = 0.0f;
};

namespace ver
{
	class ShadowCameraCBuf : public Bindable
	{
	protected:
		struct Transform
		{
			DirectX::XMMATRIX ViewProj;
		};
	public:
		ShadowCameraCBuf(Graphics& gfx, UINT slot = 1u);
		void Bind(Graphics& gfx) noxnd override;
		void Update(Graphics& gfx);
	private:
		std::unique_ptr<VertexConstantBuffer<Transform>> pVcbuf;
	};

	class BillboardCbuf : public CloningBindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX modelView;
			DirectX::XMMATRIX proj;
		};
	public:
		BillboardCbuf(Graphics& gfx, uint32_t slot = 0u);
	public:
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

	class DomainTransformCbuf : public CloningBindable
	{
	protected:
		struct Transforms
		{
			DirectX::XMMATRIX model;
			DirectX::XMMATRIX view;
			DirectX::XMMATRIX proj;
		};
	public:
		DomainTransformCbuf(Graphics& gfx, UINT slot = 0u);
		void Bind(Graphics& gfx) noxnd override;
		void InitializeParentReference(const Drawable& parent) noexcept override;
		std::unique_ptr<CloningBindable> Clone() const noexcept override;
	protected:
		void UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept;
		Transforms GetTransforms(Graphics& gfx) noexcept;
	private:
		static std::unique_ptr<DomainConstantBuffer<Transforms>> pVcbuf;
		const Drawable* pParent = nullptr;
	};
}
