#include "RePrism.h"
#include "Prism.h"
#include "BindableBase.h"

RePrism::RePrism(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_real_distribution<float>& bdist,
	std::uniform_int_distribution<int>& longdist)
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
	namespace dx = DirectX;
	auto&& vertex = DV::VertexLayout{}
		+DV::Type::Position3D
		+ DV::Type::BGRAColor;

	auto model = Prism::MakeTesselated(longdist(rng), vertex);
	// set vertex colors for mesh
	model.vertices[0].Set<DV::Type::BGRAColor>({ 0,255,255,0 });
	model.vertices[1].Set<DV::Type::BGRAColor>({ 0,255,255,0 });
	model.vertices[2].Set<DV::Type::BGRAColor>({ 0,124,252,0 });
	model.vertices[3].Set<DV::Type::BGRAColor>({ 0,127,255,212 });
	model.vertices[4].Set<DV::Type::BGRAColor>({ 0,255,255,80 });
	model.vertices[5].Set<DV::Type::BGRAColor>({ 0,255,10,0 });
	model.vertices[6].Set<DV::Type::BGRAColor>({ 0,0,255,0 });
	// deform mesh linearly
	model.Deform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

	AddBind(std::make_shared<VertexBuffer>(gfx, model.vertices));

	auto pvs = std::make_shared<VertexShader>(gfx, L"ColorBlendVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(std::make_shared<PixelShader>(gfx, L"ColorBlendPS.cso"));
	AddBind(std::make_shared<IndexBuffer>(gfx, model.indices));
	AddBind(std::make_shared<InputLayout>(gfx, model.vertices.GetLayout().GetD3DLayout(), pvsbc));
	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));

	// store deformation
	dx::XMStoreFloat3x3(
		&mt,
		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
	);
}

void RePrism::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX RePrism::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMLoadFloat3x3(&mt)*
		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}
