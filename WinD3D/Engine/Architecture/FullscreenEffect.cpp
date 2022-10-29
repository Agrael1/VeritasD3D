#include "FullscreenEffect.h"

void ver::FullscreenEffect::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
{
	auto ctx = GetContext(gfx);
	ctx->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	ctx->IASetInputLayout(nullptr);
	ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
