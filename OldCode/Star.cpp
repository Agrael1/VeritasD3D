#include "Star.h"
#include <Scene/Plane.h>

Star::Star(Graphics& gfx, float radius)
	:colorBuffer(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u))
{
	namespace dx = DirectX;
	const auto geometryTag = "$sstar." + std::to_string(radius);
	auto model = Plane::Make();
	model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorReplicate(radius)));
	pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
	pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
	pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	{
		Technique solid;
		Step only("lambertian");

		only.AddBindable(RasterizerState::Resolve(gfx, true));
		auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
		auto pvsbc = pvs->GetBytecode();
		only.AddBindable(std::move(pvs));

		only.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));


		only.AddBindable(colorBuffer);
		only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
		only.AddBindable(std::make_shared<TransformCbuf>(gfx));

		solid.AddStep(std::move(only));
		AddTechnique(std::move(solid));
	}
}

void Star::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}
void Star::SetColor(DirectX::XMFLOAT3 color) noexcept
{
	colorConst.color = color;
}
void Star::UpdateColor(Graphics& gfx) noexcept
{
	colorBuffer->Update(gfx, colorConst);
}
DirectX::XMMATRIX Star::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixRotationRollPitchYaw(1.0f,0.0f,0.0f)*DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
