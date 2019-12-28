#include "SolidSphere.h"
#include "BindableCommons.h"
#include "Sphere.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius, DirectX::XMFLOAT3 color)
	:color(color)
{
	namespace dx = DirectX;
	auto tag = "SolidSphere";
	auto model = Sphere::Make();
	model.Deform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind(VertexBuffer::Resolve(gfx, tag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, tag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "SolidPS.cso"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color;
		float padding;
	} colorConst;
	colorConst.color = color;

	AddBind(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));
	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void SolidSphere::SetPos(DirectX::XMFLOAT3 pos) noexcept
{
	this->pos = pos;
}
void SolidSphere::SetColor(DirectX::XMFLOAT3 color) noexcept
{
	this->color = color;
}
void SolidSphere::Update(float dt) noexcept
{

}
DirectX::XMMATRIX SolidSphere::GetTransformXM() const noexcept
{
	return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
}
