#include "ReSurface.h"
#include "Sheet.h"
#include "Plane.h"
#include "Texture.h"
#include "Sampler.h"
#include "BindableBase.h"

Sheet::Sheet(Graphics & gfx, std::mt19937 & rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
	:
	r(rdist(rng)),
	droll(ddist(rng)),
	dpitch(ddist(rng)),
	dyaw(ddist(rng)),
	dphi(odist(rng)),
	dtheta(odist(rng)),
	dchi(odist(rng)),
	chi(adist(rng)),
	theta(adist(rng)),
	phi(adist(rng))
{
	if (!IsStaticInitialized())
	{
		auto&& vertex = DV::VertexLayout{}
			+ DV::Type::Position3D
			+ DV::Type::Texture2D;

		auto model = Plane::Make(vertex);

		model.vertices[0].Set<DV::Type::Texture2D>({ 0.0f,0.0f });
		model.vertices[1].Set<DV::Type::Texture2D>({ 1.0f,0.0f });
		model.vertices[2].Set<DV::Type::Texture2D>({ 0.0f,1.0f });
		model.vertices[3].Set<DV::Type::Texture2D>({ 1.0f,1.0f });

		AddStaticBind(std::make_unique<Texture>(gfx, ReSurface(L"C:\\Users\\aa\\Desktop\\Marble.jpg")));
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.vertices));
		AddStaticBind(std::make_unique<Sampler>(gfx));

		auto pvs = std::make_unique<VertexShader>(gfx, L"TextureVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"TexturePS.cso"));
		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.indices));

		AddStaticBind(std::make_unique<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}
	else
	{
		SetIndexFromStatic();
	}
	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}
void Sheet::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Sheet::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
