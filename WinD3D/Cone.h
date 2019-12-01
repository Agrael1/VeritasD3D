#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <optional>

class Cone
{
public:
	static IndexedTriangleList MakeTesselated(unsigned short longDiv, std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		assert(longDiv >= 3);

		const auto base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = DirectX::XM_2PI / longDiv;

		if (!layout)
		{
			layout = DV::VertexLayout{}
			+DV::Type::Position3D;
		}
		DV::VertexBuffer vertices(std::move(*layout));
		vertices.Reserve((size_t)longDiv + 2);

		unsigned short iLong = 0;
		DirectX::XMFLOAT3 pos{};
		// base vertices
		for (; iLong < longDiv; iLong++)
		{
			auto v = DirectX::XMVector3Transform(
				base,
				DirectX::XMMatrixRotationZ(longitudeAngle * iLong)
			);
			DirectX::XMStoreFloat3(&pos, v);
			vertices[iLong].Set<DV::Type::Position3D>(std::move(pos));
		}
		// the center
		vertices[iLong++].Set<DV::Type::Position3D>({ 0.0f,0.0f,-1.0f });
		const auto iCenter = (unsigned short)(vertices.Count() - 1);
		// the tip :darkness:
		vertices[iLong].Set<DV::Type::Position3D>({ 0.0f,0.0f,1.0f });
		const auto iTip = (unsigned short)(vertices.Count() - 1);


		// base indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}

		// cone indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iLong);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iTip);
		}

		return{ std::move(vertices),std::move(indices) };
	}
	static IndexedTriangleList Make(std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		return MakeTesselated(24, layout);
	}
};