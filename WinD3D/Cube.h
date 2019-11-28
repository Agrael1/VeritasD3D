#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include <optional>

class Cube
{
public:
	static IndexedTriangleList Make(std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		constexpr float side = 1.0f / 2.0f;

		using Element = DV::VertexLayout::ElementType;
		if (!layout)
		{
			layout = DV::VertexLayout{}
			+Element::Position3D;
		}
		DV::VertexBuffer vertices(std::move(*layout));

		vertices.Reserve(8u);
		vertices[0].Set<Element::Position3D>({ -side, -side, -side }); // 0
		vertices[1].Set<Element::Position3D>({side, -side, -side}); // 1
		vertices[2].Set<Element::Position3D>({-side, side, -side}); // 2
		vertices[3].Set<Element::Position3D>({side, side, -side}); // 3
		vertices[4].Set<Element::Position3D>({-side, -side, side}); // 4
		vertices[5].Set<Element::Position3D>({side, -side, side}); // 5
		vertices[6].Set<Element::Position3D>({-side, side, side}); // 6
		vertices[7].Set<Element::Position3D>({side, side, side}); // 7

		return{
			std::move(vertices),{
				0,2,1, 2,3,1,
				1,3,5, 3,7,5,
				2,6,3, 3,6,7,
				4,5,7, 4,7,6,
				0,4,2, 2,4,6,
				0,1,4, 1,5,4
			}
		};
	}
	static IndexedTriangleList MakeSkinned(std::optional<DV::VertexLayout> layout = std::nullopt)
	{
		namespace dx = DirectX;
		constexpr float side = 1.0f / 2.0f;

		if (!layout)
		{
			layout = DV::VertexLayout{}
				+ DV::Type::Position3D
				+ DV::Type::Texture2D;
		}
		DV::VertexBuffer vertices(std::move(*layout));
		vertices.Reserve(14u);

		vertices[0].Set<DV::Type::Position3D>({ -side,-side,-side });
		vertices[0].Set<DV::Type::Texture2D>({ 2.0f / 3.0f,0.0f / 4.0f });
		vertices[1].Set<DV::Type::Position3D>({ side,-side,-side });
		vertices[1].Set<DV::Type::Texture2D>({ 1.0f / 3.0f,0.0f / 4.0f });
		vertices[2].Set<DV::Type::Position3D>({ -side,side,-side });
		vertices[2].Set<DV::Type::Texture2D>({ 2.0f / 3.0f,1.0f / 4.0f });
		vertices[3].Set<DV::Type::Position3D>({ side,side,-side });
		vertices[3].Set<DV::Type::Texture2D>({ 1.0f / 3.0f,1.0f / 4.0f });
		vertices[4].Set<DV::Type::Position3D>({ -side,-side,side });
		vertices[4].Set<DV::Type::Texture2D>({ 2.0f / 3.0f,3.0f / 4.0f });
		vertices[5].Set<DV::Type::Position3D>({ side,-side,side });
		vertices[5].Set<DV::Type::Texture2D>({ 1.0f / 3.0f,3.0f / 4.0f });
		vertices[6].Set<DV::Type::Position3D>({ -side,side,side });
		vertices[6].Set<DV::Type::Texture2D>({ 2.0f / 3.0f,2.0f / 4.0f });
		vertices[7].Set<DV::Type::Position3D>({ side,side,side });
		vertices[7].Set<DV::Type::Texture2D>({ 1.0f / 3.0f,2.0f / 4.0f });
		vertices[8].Set<DV::Type::Position3D>({ -side,-side,-side });
		vertices[8].Set<DV::Type::Texture2D>({ 2.0f / 3.0f,4.0f / 4.0f });
		vertices[9].Set<DV::Type::Position3D>({ side,-side,-side });
		vertices[9].Set<DV::Type::Texture2D>({ 1.0f / 3.0f,4.0f / 4.0f });
		vertices[10].Set<DV::Type::Position3D>({ -side,-side,-side });
		vertices[10].Set<DV::Type::Texture2D>({ 3.0f / 3.0f,1.0f / 4.0f });
		vertices[11].Set<DV::Type::Position3D>({ -side,-side,side });
		vertices[11].Set<DV::Type::Texture2D>({ 3.0f / 3.0f,2.0f / 4.0f });
		vertices[12].Set<DV::Type::Position3D>({ side,-side,-side });
		vertices[12].Set<DV::Type::Texture2D>({ 0.0f / 3.0f,1.0f / 4.0f });
		vertices[13].Set<DV::Type::Position3D>({ side,-side,side });
		vertices[13].Set<DV::Type::Texture2D>({ 0.0f / 3.0f,2.0f / 4.0f });

		return{
			std::move(vertices),{
				0,2,1,   2,3,1,
				4,8,5,   5,8,9,
				2,6,3,   3,6,7,
				4,5,7,   4,7,6,
				2,10,11, 2,11,6,
				12,3,7,  12,7,13
			}
		};
	}
};