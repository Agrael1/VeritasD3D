#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <optional>


class Prism
{
public:
	static IndexedTriangleList MakeTesselated(unsigned longDiv = 3u, std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		namespace dx = DirectX;
		assert(longDiv >= 3);

		const auto base = dx::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const auto offset = dx::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = dx::XM_2PI / longDiv;

		DV::VertexBuffer vertices{ std::move(*layout) };
		vertices.Reserve(2 * longDiv + 2);
		// near center
		vertices[0].Set<DV::Type::Position3D>({ 0.0f,0.0f,-1.0f });
		const auto iCenterNear = (unsigned short)(vertices.Count() - 1);
		// far center
		vertices[1].Set<DV::Type::Position3D>({ 0.0f,0.0f,1.0f });
		const auto iCenterFar = (unsigned short)(vertices.Count() - 1);

		DirectX::XMFLOAT3 pos{};
		// base vertices
		for (unsigned iLong = 0; iLong < longDiv; iLong++)
		{
			// near base
			{
				auto v = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				DirectX::XMStoreFloat3(&pos, v);
				vertices[2 * iLong].Set<DV::Type::Position3D>(std::move(pos));
			}
			// far base
			{
				auto v = dx::XMVector3Transform(
					base,
					dx::XMMatrixRotationZ(longitudeAngle * iLong)
				);
				v = dx::XMVectorAdd(v, offset);
				DirectX::XMStoreFloat3(&pos, v);
				vertices[2 * iLong + 1].Set<DV::Type::Position3D>(std::move(pos));
			}
		}

		// side indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indices.push_back(i + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back((i + 3) % mod + 2);
			indices.push_back(i + 1 + 2);
		}

		// base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const auto i = iLong * 2;
			const auto mod = longDiv * 2;
			indices.push_back(i + 2);
			indices.push_back(iCenterNear);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(iCenterFar);
			indices.push_back(i + 1 + 2);
			indices.push_back((i + 3) % mod + 2);
		}

		return{ std::move(vertices),std::move(indices) };
	}
	static IndexedTriangleList Make(std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		using Element = DV::VertexLayout::ElementType;
		if (!layout)
		{
			layout = DV::VertexLayout{}
			+Element::Position3D;
		}
		return MakeTesselated(24, std::move(layout));
	}
};