#pragma once
#include <Engine/Graphics.h>
#include <Engine/Architecture/Drawable.h>
#include <Framework/noexcept_if.h>

class Material;
class FrameCommander;
struct aiMesh;


class Mesh : public Drawable
{
public:
	Mesh(Graphics& gfx, const Material& mat, const aiMesh& mesh, float scale = 1.0f) noxnd;
	DirectX::XMMATRIX GetTransformXM() const noexcept override;
	void Submit(DirectX::FXMMATRIX accumulatedTranform) const noxnd;
private:
	mutable DirectX::XMFLOAT4X4 transform;
};