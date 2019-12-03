#include "ReSurface.h"
#include "Sheet.h"
#include "Plane.h"
#include "Texture.h"
#include "Sampler.h"
#include "BindableBase.h"

Sheet::Sheet(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
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

	auto&& vertex = DV::VertexLayout{}
		+DV::Type::Position3D
		+ DV::Type::Texture2D;

	auto model = Plane::Make(vertex);

	model.vertices[0].Set<DV::Type::Texture2D>({ 0.0f,0.0f });
	model.vertices[1].Set<DV::Type::Texture2D>({ 1.0f,0.0f });
	model.vertices[2].Set<DV::Type::Texture2D>({ 0.0f,1.0f });
	model.vertices[3].Set<DV::Type::Texture2D>({ 1.0f,1.0f });

	AddBind(std::make_shared<Texture>(gfx, ReSurface(L"C:\\Users\\aa\\Desktop\\Marble.jpg")));
	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));
	AddBind(std::make_shared<Sampler>(gfx));

	auto pvs = std::make_shared<VertexShader>(gfx, L"TextureVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(gfx, L"TexturePS.cso"));
	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));
	AddBind(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));
	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
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
