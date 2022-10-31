//#include "Box.h"
//#include "BindableCommons.h"
//#include "Cube.h"
//
//
//Box::Box(Graphics& gfx,
//	std::mt19937& rng,
//	std::uniform_real_distribution<float>& adist,
//	std::uniform_real_distribution<float>& ddist,
//	std::uniform_real_distribution<float>& odist,
//	std::uniform_real_distribution<float>& rdist,
//	std::uniform_real_distribution<float>& bdist)
//	:
//	r(rdist(rng)),
//	droll(ddist(rng)),
//	dpitch(ddist(rng)),
//	dyaw(ddist(rng)),
//	dphi(odist(rng)),
//	dtheta(odist(rng)),
//	dchi(odist(rng)),
//	chi(adist(rng)),
//	theta(adist(rng)),
//	phi(adist(rng))
//{
//	namespace dx = DirectX;
//
//	const auto model = Cube::Make();
//	auto tag = "Box";
//	AddBind(VertexBuffer::Resolve(gfx, tag, model.vertices));
//
//	auto pvs = VertexShader::Resolve(gfx, "ColorIndexVS.cso");
//	auto pvsbc = pvs->GetBytecode();
//	AddBind(std::move(pvs));
//
//	AddBind(PixelShader::Resolve(gfx, "ColorIndexPS.cso"));
//	AddBind(IndexBuffer::Resolve(gfx, tag, model.indices));
//
//	struct PixelShaderConstants
//	{
//		struct
//		{
//			float r;
//			float g;
//			float b;
//			float a;
//		} face_colors[8];
//	};
//	const PixelShaderConstants cb2 =
//	{
//		{
//			{ 1.0f,1.0f,1.0f },
//			{ 1.0f,0.0f,0.0f },
//			{ 0.0f,1.0f,0.0f },
//			{ 1.0f,1.0f,0.0f },
//			{ 0.0f,0.0f,1.0f },
//			{ 1.0f,0.0f,1.0f },
//			{ 0.0f,1.0f,1.0f },
//			{ 0.0f,0.0f,0.0f },
//		}
//	};
//	AddBind(PixelConstantBuffer<PixelShaderConstants>::Resolve(gfx, cb2));
//	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
//	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//
//
//	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
//
//	// model deformation transform (per instance, not stored as bind)
//	dx::XMStoreFloat3x3(
//		&mt,
//		dx::XMMatrixScaling(1.0f, 1.0f, bdist(rng))
//	);
//}
//
//void Box::Update(float dt) noexcept
//{
//	roll += droll * dt;
//	pitch += dpitch * dt;
//	yaw += dyaw * dt;
//	theta += dtheta * dt;
//	phi += dphi * dt;
//	chi += dchi * dt;
//}
//
//DirectX::XMMATRIX Box::GetTransformXM() const noexcept
//{
//	namespace dx = DirectX;
//	return dx::XMLoadFloat3x3(&mt) *
//		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
//		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
//		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
//}