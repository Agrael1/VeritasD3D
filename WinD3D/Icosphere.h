#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <numeric>
#include <optional>

class Icosphere
{
public:
	static IndexedTriangleList Make(std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		using Element = DV::VertexLayout::ElementType;
		using namespace DirectX;
		if (!layout)
		{
			layout = DV::VertexLayout{}
			+Element::Position3D;
		}
		DV::VertexBuffer vb{ std::move(*layout), 12};

		const float t = (1.0f + std::sqrtf(5.0f)) / 2.0f;
		
		vb[0].Set<Element::Position3D>({ -1.0f, t, 0.0f });
		vb[1].Set<Element::Position3D>({ 1.0f, t, 0.0f });
		vb[2].Set<Element::Position3D>({ -1.0f, -t, 0.0f });
		vb[3].Set<Element::Position3D>({ 1.0f, -t, -0.0f });
		  
		vb[4].Set<Element::Position3D>({ 0.0f, -1.0f, t });
		vb[5].Set<Element::Position3D>({ 0.0f, 1.0f, t });
		vb[6].Set<Element::Position3D>({ 0.0f, -1.0f, -t });
		vb[7].Set<Element::Position3D>({ 0.0f, 1.0f, -t });		
		
		vb[8].Set<Element::Position3D>({ t, 0.0f, -1.0f });
		vb[9].Set<Element::Position3D>({ t, 0.0f, 1.0f });
		vb[10].Set<Element::Position3D>({ -t, 0.0f, -1.0f });
		vb[11].Set<Element::Position3D>({ -t, 0.0f, 1.0f });

		return{
			std::move(vb),{
				0, 11,5,	0, 5, 1,	0, 1, 7,	0, 7,10,	0,10,11,
				1, 5, 9,	5, 11,4,	11,10,2,	10,7, 6,	7, 1, 8,
				3, 9, 4,	3, 4, 2,	3, 2, 6,	3, 6, 8,	3, 8, 9,
				4, 9, 5,	2, 4,11,	6, 2,10,	8, 6, 7,	9, 8, 1
			}
		};
	}

	static IndexedTriangleList MakeIndependent(std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		auto temp = Make(layout);
		DV::VertexBuffer ReVertices{ std::move(temp.vertices.GetLayout()) };
		ReVertices.Reserve(temp.indices.size());
		UINT j = 0;
		for (auto i : temp.indices)
		{
			ReVertices[j++] = temp.vertices[i];
		}
		std::vector<unsigned short> ReIndices(temp.indices.size());
		std::iota(ReIndices.begin(), ReIndices.end(), 0);

		return{ std::move(ReVertices), std::move(ReIndices) };
	}
};