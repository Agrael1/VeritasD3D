#include "Melon.h"
#include "BindableBase.h"
#include "Sphere.h"


Melon::Melon(Graphics& gfx,
	std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	std::uniform_int_distribution<int>& longdist,
	std::uniform_int_distribution<int>& latdist)
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
	auto tag = "Melon";

	auto&& vertex = DV::VertexLayout{}
	+DV::Type::Position3D;

	auto model = Sphere::MakeTesselated(vertex, latdist(rng), longdist(rng));
	// deform vertices of model by linear transformation
	model.Deform(dx::XMMatrixScaling(1.0f, 1.0f, 1.2f));

	AddBind(VertexBuffer::Resolve(gfx, tag, model.vertices));
	AddBind(IndexBuffer::Resolve(gfx, tag, model.indices));

	auto pvs = VertexShader::Resolve(gfx, "ColorIndexVS.cso");
	auto pvsbc = pvs->GetBytecode();
	AddBind(std::move(pvs));

	AddBind(PixelShader::Resolve(gfx, "ColorIndexPS.cso"));

	struct PixelShaderConstants
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		} face_colors[8];
	};
	const PixelShaderConstants cb2 =
	{
		{
			{ 1.0f,1.0f,1.0f },
			{ 1.0f,0.0f,0.0f },
			{ 0.0f,1.0f,0.0f },
			{ 1.0f,1.0f,0.0f },
			{ 0.0f,0.0f,1.0f },
			{ 1.0f,0.0f,1.0f },
			{ 0.0f,1.0f,1.0f },
			{ 0.0f,0.0f,0.0f },
		}
	};
	AddBind(PixelConstantBuffer<PixelShaderConstants>::Resolve(gfx, cb2));
	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
}

void Melon::Update(float dt) noexcept
{
	roll += droll * dt;
	pitch += dpitch * dt;
	yaw += dyaw * dt;
	theta += dtheta * dt;
	phi += dphi * dt;
	chi += dchi * dt;
}

DirectX::XMMATRIX Melon::GetTransformXM() const noexcept
{
	namespace dx = DirectX;
	return dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
}