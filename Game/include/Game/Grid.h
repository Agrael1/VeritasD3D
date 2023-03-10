#pragma once
#include <Engine/Scene/Plane.h>
#include <Engine/Rendering/Drawable.h>
#include <Engine/Bindable/BindableCommons.h>

class Grid : public Drawable
{
public:
	Grid() = default;
	void Initialize(Graphics& gfx)
	{
		namespace dx = DirectX;
		const auto geometryTag = "$sgrid";
		auto model = Plane::MakeTesselatedLined(100, 100);
		model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorReplicate(100.0f)));
		pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
		pIndices = ver::IndexBuffer::Resolve(gfx, geometryTag, model.indices);
		topology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;

		{
			Technique solid;
			Step only("lambertian");

			auto pvs = ver::VertexShader::Resolve(gfx, "solid.vs.cso");
			auto pvsbc = pvs->GetBytecode();
			only.AddBindable(std::move(pvs));

			only.AddBindable(ver::PixelShader::Resolve(gfx, "solid.ps.cso"));

			struct PSColorConstant
			{
				dx::XMFLOAT3 color = { 1.f,1.f,1.f };
				float padding = 0;
			} colorConst;
			only.AddBindable(std::make_shared<ver::PixelConstantBuffer<PSColorConstant>>(gfx, colorConst, 1u));
			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.layout(), pvsbc));
			only.AddBindable(std::make_shared<TransformCbuf>(gfx));

			solid.AddStep(std::move(only));
			AddTechnique(std::move(solid));
		}
	}
public:
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PIDIV2, 0.0f, 0.0f);
	}
};