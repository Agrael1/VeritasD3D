#include "Pyramid.h"
#include "BindableCommons.h"
#include "Cone.h"


Pyramid::Pyramid(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist)
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

	auto&& layout = DV::VertexLayout{}
		+DV::Type::Position3D
		+ DV::Type::BGRAColor;
	auto tag = "Pyramid";
	auto model = Cone::MakeTesselated(4, layout);
	// set vertex colors for mesh
	model.vertices[0].Set<DV::Type::BGRAColor>({ 0, 255,255,0 });
	model.vertices[1].Set<DV::Type::BGRAColor>({ 0, 255,255,0 });
	model.vertices[2].Set<DV::Type::BGRAColor>({ 0, 124,252,0 });
	model.vertices[3].Set<DV::Type::BGRAColor>({ 0, 127,255,212 });
	model.vertices[4].Set<DV::Type::BGRAColor>({ 0, 255,255,80 });
	model.vertices[5].Set<DV::Type::BGRAColor>({ 0, 255,10,0 });
	// deform mesh linearly
	model.Deform(dx::XMMatrixScaling(1.0f, 1.0f, 0.7f));

	AddBind(VertexBuffer::Resolve(gfx, tag, model.vertices));

	auto pvs = VertexShader::Resolve(gfx, "ColorBlendVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "ColorBlendPS.cso"));
	AddBind(IndexBuffer::Resolve(gfx, tag, model.indices));
	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void Pyramid::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Pyramid::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}