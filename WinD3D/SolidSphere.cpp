#include "SolidSphere.h"
#include "BindableBase.h"
#include "Sphere.h"

SolidSphere::SolidSphere(Graphics& gfx, float radius, DirectX::XMFLOAT3 color)
	:color(color)
{
	namespace dx = DirectX;
	auto model = Sphere::Make();
	model.Deform(dx::XMMatrixScaling(radius, radius, radius));
	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));
	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));

	auto pvs = std::make_unique<VertexShader>(gfx, L"SolidVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(gfx, L"SolidPS.cso"));

	struct PSColorConstant
	{
		dx::XMFLOAT3 color;
		float padding;
	} colorConst;
	colorConst.color = color;

	AddBind(std::make_shared<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));
	AddBind(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));
	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
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
