#include "SolidSphere.h"
#include "BindableBase.h"
#include "Sphere.h"

SolidSphere::SolidSphere(Graphics & gfx, float radius, DirectX::XMFLOAT3 color)
	:color(color)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		auto model = Sphere::Make();
		model.Deform(dx::XMMatrixScaling(radius, radius, radius));
		AddBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		AddIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"SolidVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"SolidPS.cso"));

		struct PSColorConstant
		{
			dx::XMFLOAT3 color;
			float padding;
		} colorConst;
		colorConst.color = color;
		
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSColorConstant>>(gfx, colorConst));

		AddStaticBind(std::make_unique<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
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
