#include "Mesh.h"

// Mesh
Mesh::Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale) noxnd
	:Drawable(gfx, mat, mesh, scale)
{}

void Mesh::Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTranform) const noxnd
{
	DirectX::XMStoreFloat4x4(&transform, accumulatedTranform);
	Drawable::Submit(frame);
}

DirectX::XMMATRIX Mesh::GetTransformXM() const noexcept
{
	return DirectX::XMLoadFloat4x4(&transform);
}
