#pragma once
#include "Plane.h"
#include <Architecture/Drawable.h>
#include <Architecture/BindableCommons.h>

class Grid : public Drawable
{
public:
	Grid(Graphics& gfx)
	{
		namespace dx = DirectX;
		const auto geometryTag = "$sgrid";
		auto model = Plane::MakeTesselatedLined(10, 10);
		model.Deform(dx::XMMatrixScalingFromVector(dx::XMVectorReplicate(5.0f)));
		pVertices = VertexBuffer::Resolve(gfx, geometryTag, model.vertices);
		pIndices = IndexBuffer::Resolve(gfx, geometryTag, model.indices);
		pTopology = Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP);

		{
			Technique solid;
			Step only("lambertian");

			auto pvs = VertexShader::Resolve(gfx, "Solid_VS.cso");
			auto pvsbc = pvs->GetBytecode();
			only.AddBindable(std::move(pvs));

			only.AddBindable(PixelShader::Resolve(gfx, "Solid_PS.cso"));

			struct PSColorConstant
			{
				dx::XMFLOAT3 color = { 1.f,1.f,1.f };
				float padding = 0;
			} colorConst;
			only.AddBindable(PixelConstantBuffer<PSColorConstant>::Resolve(gfx, colorConst, 1u));
			only.AddBindable(InputLayout::Resolve(gfx, model.vertices.GetLayout(), pvsbc));
			only.AddBindable(std::make_shared<TransformCbuf>(gfx));

			solid.AddStep(std::move(only));
			AddTechnique(std::move(solid));
		}
	}
public:
	DirectX::XMMATRIX GetTransformXM() const noexcept override
	{
		return DirectX::XMMatrixRotationRollPitchYaw(DirectX::XM_PIDIV2,0.0f,0.0f);
	}
};