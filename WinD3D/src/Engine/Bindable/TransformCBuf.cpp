#include <Engine/Bindable/TransformCBuf.h>
#include <Engine/Graphics.h>
#include <Shared/Checks.h>

TransformCbuf::TransformCbuf(Graphics& gfx, UINT slot)
{
	if (!pVcbuf)
	{
		pVcbuf = std::make_unique<ver::VertexConstantBuffer<Transforms>>(gfx, slot);
	}
}

void TransformCbuf::Bind(Graphics& gfx) noxnd
{
	UpdateBindImpl(gfx, GetTransforms(gfx));
	ver::check_context();
}
void TransformCbuf::InitializeParentReference(const Drawable& parent) noexcept
{
	pParent = &parent;
}
std::unique_ptr<CloningBindable> TransformCbuf::Clone() const noexcept
{
	return std::make_unique<TransformCbuf>(*this);
}
void TransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
{
	assert(pParent != nullptr);
	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}
TransformCbuf::Transforms TransformCbuf::GetTransforms(Graphics& gfx) noexcept
{
	assert(pParent != nullptr);
	const auto model = pParent->GetTransformXM();
	const auto modelView = model * gfx.GetCamera();
	return {
		DirectX::XMMatrixTranspose(model),
		DirectX::XMMatrixTranspose(modelView),
		DirectX::XMMatrixTranspose(
			modelView *
			gfx.GetProjection()
		)
	};
}

std::unique_ptr<ver::VertexConstantBuffer<TransformCbuf::Transforms>> TransformCbuf::pVcbuf;

SkyboxTransformCbuf::SkyboxTransformCbuf(Graphics& gfx, UINT slot)
	:
	pVcbuf{ std::make_unique<ver::VertexConstantBuffer<Transforms>>(gfx,slot) }
{}

void SkyboxTransformCbuf::Bind(Graphics& gfx) noxnd
{
	UpdateBindImpl(gfx, GetTransforms(gfx));
	ver::check_context();
}

void SkyboxTransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd
{
	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}

SkyboxTransformCbuf::Transforms SkyboxTransformCbuf::GetTransforms(Graphics& gfx) noxnd
{
	if (c++ == 2)
	{
		c = 1;
		if (f >= DirectX::XM_2PI)f -= DirectX::XM_2PI;
		f += gfx.GetFrameStep() / 4.0f;
	}
	return {
		DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection()),
		DirectX::XMMatrixTranspose(DirectX::XMMatrixRotationRollPitchYaw(f,0,0))
	};
}


XSkyboxTransformCbuf::XSkyboxTransformCbuf(Graphics& gfx, UINT slot)
	:
	pVcbuf{ std::make_unique<ver::VertexConstantBuffer<Transforms>>(gfx,slot) }
{}

void XSkyboxTransformCbuf::Bind(Graphics& gfx) noxnd
{
	UpdateBindImpl(gfx, GetTransforms(gfx));
	ver::check_context();
}

void XSkyboxTransformCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noxnd
{
	pVcbuf->Update(gfx, tf);
	pVcbuf->Bind(gfx);
}

XSkyboxTransformCbuf::Transforms XSkyboxTransformCbuf::GetTransforms(Graphics& gfx) noxnd
{
	return {
		DirectX::XMMatrixTranspose(gfx.GetCamera() * gfx.GetProjection()),
		DirectX::XMMatrixTranspose(DirectX::XMMatrixIdentity())
	};
}

namespace dx = DirectX;

namespace ver
{
	ShadowCameraCBuf::ShadowCameraCBuf(Graphics& gfx, UINT slot)
		:
		pVcbuf{ std::make_unique<VertexConstantBuffer<Transform>>(gfx,slot) }
	{}
	void ShadowCameraCBuf::Bind(Graphics& gfx) noxnd
	{
		pVcbuf->Bind(gfx);
	}
	void ShadowCameraCBuf::Update(Graphics& gfx)
	{
		using namespace dx;
		const auto pos = -gfx.GetShadowCamPos();
		const Transform t{
			dx::XMMatrixTranspose(
				dx::XMMatrixTranslationFromVector(pos)
			)
		};
		pVcbuf->Update(gfx, t);
	}


	BillboardCbuf::BillboardCbuf(Graphics& gfx, UINT slot)
	{
		if (!pVcbuf)
		{
			pVcbuf = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
		}
	}

	void BillboardCbuf::Bind(Graphics& gfx) noxnd
	{
		(UpdateBindImpl(gfx, GetTransforms(gfx)));
		ver::check_context();
	}
	void BillboardCbuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		pParent = &parent;
	}
	std::unique_ptr<CloningBindable> BillboardCbuf::Clone() const noexcept
	{
		return std::make_unique<BillboardCbuf>(*this);
	}
	void BillboardCbuf::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
	{
		assert(pParent != nullptr);
		pVcbuf->Update(gfx, tf);
		pVcbuf->Bind(gfx);
	}
	BillboardCbuf::Transforms BillboardCbuf::GetTransforms(Graphics& gfx) noexcept
	{
		assert(pParent != nullptr);
		const auto model = pParent->GetTransformXM();
		const auto modelView = model * gfx.GetCamera();
		return {
			DirectX::XMMatrixTranspose(modelView),
			DirectX::XMMatrixTranspose(gfx.GetProjection())
		};
	}

	std::unique_ptr<VertexConstantBuffer<BillboardCbuf::Transforms>> BillboardCbuf::pVcbuf;
}