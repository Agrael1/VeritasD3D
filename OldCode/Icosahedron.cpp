//#include "Icosahedron.h"
//#include "Icosphere.h"
//#include "BindableCommons.h"
//
//Icosahedron::Icosahedron(Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist, DirectX::XMFLOAT3 material)
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
//	phi(adist(rng)),
//	radius(bdist(rng))
//{
//	namespace dx = DirectX;
//
//	auto&& Vertex = DV::VertexLayout{}
//		+DV::Type::Position3D
//		+ DV::Type::Normal;
//
//	auto tag = "Icosphere";
//	auto model = Icosphere::MakeIndependent(Vertex);
//	// normalize the sphere
//	const float scale = 2 / (3.0f + std::sqrtf(5));
//	model.Deform(dx::XMMatrixScaling(scale, scale, scale));
//	model.CalcNormalsIndependentFlat();
//
//	AddBind(VertexBuffer::Resolve(gfx, tag, model.vertices));
//
//	auto pvs = VertexShader::Resolve(gfx, "PhongVS.cso");
//	auto pvsbc = pvs->GetBytecode();
//	AddBind(std::move(pvs));
//
//	AddBind(PixelShader::Resolve(gfx, "PhongPS.cso"));
//	AddBind(IndexBuffer::Resolve(gfx, tag, model.indices));
//	AddBind(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
//	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
//	AddBind(std::make_shared<TransformCbuf>(gfx, *this));
//
//	struct PSMaterialConstant
//	{
//		alignas(16)DirectX::XMFLOAT3 color;
//		float specularIntensity = 0.6f;
//		float specularPower = 30.0f;
//		float padding[2];
//	}colorConst;
//	colorConst.color = material;
//	AddBind(PixelConstantBuffer<PSMaterialConstant>::Resolve(gfx, colorConst, 1u));
//
//	// Per instance scaling
//	dx::XMStoreFloat3x3(
//		&mt,
//		dx::XMMatrixScaling(radius, radius, radius)
//	);
//}
//
//void Icosahedron::Update(float dt) noexcept
//{
//	roll += droll * dt;
//	pitch += dpitch * dt;
//	yaw += dyaw * dt;
//	theta += dtheta * dt;
//	phi += dphi * dt;
//	chi += dchi * dt;
//}
//
//DirectX::XMMATRIX Icosahedron::GetTransformXM() const noexcept
//{
//	namespace dx = DirectX;
//	return dx::XMLoadFloat3x3(&mt) *
//		dx::XMMatrixRotationRollPitchYaw(pitch, yaw, roll) *
//		dx::XMMatrixTranslation(r, 0.0f, 0.0f) *
//		dx::XMMatrixRotationRollPitchYaw(theta, phi, chi);
//}
