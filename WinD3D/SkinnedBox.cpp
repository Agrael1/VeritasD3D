#include "Texture.h"
#include "SkinnedBox.h"
#include "Cube.h"
#include "BindableBase.h"
#include "Sampler.h"


SkinnedBox::SkinnedBox(Graphics& gfx)
{
	namespace dx = DirectX;
	auto tag = "SkinnedBox";

	auto vertex = DV::VertexLayout{}
		+ DV::Type::Position3D
		+ DV::Type::Normal
		+ DV::Type::Texture2D;

	auto model = Cube::MakeIndependentSkinned(vertex);
	model.CalcNormalsIndependentFlat();
	model.Deform(dx::XMMatrixScaling(5.0f, 5.0f, 5.0f));

	AddBind(VertexBuffer::Resolve(gfx, tag, model.vertices));

	AddBind(Texture::Resolve(gfx, "Materials\\crate_diffuse.png"));
	AddBind(Sampler::Resolve(gfx));

	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));
	AddBind(IndexBuffer::Resolve(gfx, tag, model.indices));
	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void SkinnedBox::Update(float dt) noexcept
{

}

DirectX::XMMATRIX SkinnedBox::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return 	DirectX::XMMatrixRotationRollPitchYaw(roll, pitch, yaw) *
		DirectX::XMMatrixTranslation(x, y, z);
}