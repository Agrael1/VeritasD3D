#include "FullscreenEffect.h"

void ver::FullscreenEffect::Bind(Graphics& gfx) noexcept(!IS_DEBUG)
{
	GetContext(gfx)->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	GetContext(gfx)->IASetInputLayout(nullptr);
	GetContext(gfx)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
