#include "TransformUnified.h"

TransformUnified::TransformUnified(Graphics& gfx, UINT slotV, UINT slotP)
	:TransformCbuf(gfx, slotV)
{
	if (!pPCBuf)
	{
		pPCBuf = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
	}
}

void TransformUnified::Bind(Graphics& gfx) noexcept
{
	const auto tf = GetTransforms(gfx);
	TransformCbuf::UpdateBindImpl(gfx, tf);
	UpdateBindImpl(gfx, tf);
}
void TransformUnified::UpdateBindImpl(Graphics& gfx, const Transforms& tf) noexcept
{
	pPCBuf->Update(gfx, tf);
	pPCBuf->Bind(gfx);
}

std::unique_ptr<PixelConstantBuffer<TransformCbuf::Transforms>> TransformUnified::pPCBuf;