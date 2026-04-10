#pragma once
#include <Engine/Scene/IndexedTriangleList.h>
#include <DirectXMath.h>
#include <optional>

class Cube
{
public:
	static IndexedTriangleList Make(std::optional<ver::dv::VertexLayout> layout = std::nullopt)
	{
		constexpr float side = 1.0f / 2.0f;

		using Element = ver::dv::ElementType;
		if (!layout)
		{
			layout.emplace();
			layout->append(Element::Position3D);
		}
		ver::dv::VertexBuffer vertices(std::move(*layout));

		vertices.reserve(8u);
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
	static IndexedTriangleList MakeSkinned(std::optional<ver::dv::VertexLayout> layout = std::nullopt)
	{
		namespace dx = DirectX;
		constexpr float side = 1.0f / 2.0f;

		if (!layout)
		{
			layout = ver::dv::VertexLayout{}
				+ ver::dv::Type::Position3D
				+ ver::dv::Type::Texture2D;
		}
		ver::dv::VertexBuffer vertices(std::move(*layout));
		vertices.Reserve(14u);

		vertices[0].Set<ver::dv::Type::Position3D>({ -side,-side,-side });
		vertices[0].Set<ver::dv::Type::Texture2D>({ 2.0f / 3.0f,0.0f / 4.0f });
		vertices[1].Set<ver::dv::Type::Position3D>({ side,-side,-side });
		vertices[1].Set<ver::dv::Type::Texture2D>({ 1.0f / 3.0f,0.0f / 4.0f });
		vertices[2].Set<ver::dv::Type::Position3D>({ -side,side,-side });
		vertices[2].Set<ver::dv::Type::Texture2D>({ 2.0f / 3.0f,1.0f / 4.0f });
		vertices[3].Set<ver::dv::Type::Position3D>({ side,side,-side });
		vertices[3].Set<ver::dv::Type::Texture2D>({ 1.0f / 3.0f,1.0f / 4.0f });
		vertices[4].Set<ver::dv::Type::Position3D>({ -side,-side,side });
		vertices[4].Set<ver::dv::Type::Texture2D>({ 2.0f / 3.0f,3.0f / 4.0f });
		vertices[5].Set<ver::dv::Type::Position3D>({ side,-side,side });
		vertices[5].Set<ver::dv::Type::Texture2D>({ 1.0f / 3.0f,3.0f / 4.0f });
		vertices[6].Set<ver::dv::Type::Position3D>({ -side,side,side });
		vertices[6].Set<ver::dv::Type::Texture2D>({ 2.0f / 3.0f,2.0f / 4.0f });
		vertices[7].Set<ver::dv::Type::Position3D>({ side,side,side });
		vertices[7].Set<ver::dv::Type::Texture2D>({ 1.0f / 3.0f,2.0f / 4.0f });
		vertices[8].Set<ver::dv::Type::Position3D>({ -side,-side,-side });
		vertices[8].Set<ver::dv::Type::Texture2D>({ 2.0f / 3.0f,4.0f / 4.0f });
		vertices[9].Set<ver::dv::Type::Position3D>({ side,-side,-side });
		vertices[9].Set<ver::dv::Type::Texture2D>({ 1.0f / 3.0f,4.0f / 4.0f });
		vertices[10].Set<ver::dv::Type::Position3D>({ -side,-side,-side });
		vertices[10].Set<ver::dv::Type::Texture2D>({ 3.0f / 3.0f,1.0f / 4.0f });
		vertices[11].Set<ver::dv::Type::Position3D>({ -side,-side,side });
		vertices[11].Set<ver::dv::Type::Texture2D>({ 3.0f / 3.0f,2.0f / 4.0f });
		vertices[12].Set<ver::dv::Type::Position3D>({ side,-side,-side });
		vertices[12].Set<ver::dv::Type::Texture2D>({ 0.0f / 3.0f,1.0f / 4.0f });
		vertices[13].Set<ver::dv::Type::Position3D>({ side,-side,side });
		vertices[13].Set<ver::dv::Type::Texture2D>({ 0.0f / 3.0f,2.0f / 4.0f });

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

	static IndexedTriangleList MakeIndependentSkinned(std::optional<ver::dv::VertexLayout> layout = std::nullopt)
	{
		namespace dx = DirectX;
		constexpr float side = 1.0f / 2.0f;

		if (!layout)
		{
			layout = ver::dv::VertexLayout{}
				+ver::dv::Type::Position3D
				+ ver::dv::Type::Texture2D;
		}
		ver::dv::VertexBuffer vertices(std::move(*layout));
		vertices.Reserve(24u);

		vertices[0].Set<ver::dv::Type::Position3D>({ -side,-side,-side });
		vertices[0].Set<ver::dv::Type::Texture2D>({ 1.0f, 1.0f });
		vertices[1].Set<ver::dv::Type::Position3D>({ side,-side,-side });
		vertices[1].Set<ver::dv::Type::Texture2D>({ 0.0f, 1.0f });
		vertices[2].Set<ver::dv::Type::Position3D>({ -side,side,-side });
		vertices[2].Set<ver::dv::Type::Texture2D>({ 1.0f, 0.0f });
		vertices[3].Set<ver::dv::Type::Position3D>({ side,side,-side });
		vertices[3].Set<ver::dv::Type::Texture2D>({ 0.0f, 0.0f });


		vertices[4].Set<ver::dv::Type::Position3D>({ -side,-side,side });
		vertices[4].Set<ver::dv::Type::Texture2D>({ 0.0f, 1.0f });
		vertices[5].Set<ver::dv::Type::Position3D>({ side,-side,side });
		vertices[5].Set<ver::dv::Type::Texture2D>({ 1.0f, 1.0f });
		vertices[6].Set<ver::dv::Type::Position3D>({ -side,side,side });
		vertices[6].Set<ver::dv::Type::Texture2D>({ 0.0f, 0.0f });
		vertices[7].Set<ver::dv::Type::Position3D>({ side,side,side });
		vertices[7].Set<ver::dv::Type::Texture2D>({ 1.0f, 0.0f });


		vertices[8].Set<ver::dv::Type::Position3D>({ side,side,side });
		vertices[8].Set<ver::dv::Type::Texture2D>({ 0.0f, 0.0f });
		vertices[9].Set<ver::dv::Type::Position3D>({ side,-side,side });
		vertices[9].Set<ver::dv::Type::Texture2D>({ 0.0f, 1.0f });
		vertices[10].Set<ver::dv::Type::Position3D>({ side,-side,-side });
		vertices[10].Set<ver::dv::Type::Texture2D>({ 1.0f, 1.0f });
		vertices[11].Set<ver::dv::Type::Position3D>({ side,side,-side });
		vertices[11].Set<ver::dv::Type::Texture2D>({ 1.0f, 0.0f });


		vertices[12].Set<ver::dv::Type::Position3D>({ -side,side,-side });
		vertices[12].Set<ver::dv::Type::Texture2D>({ 0.0f, 0.0f });
		vertices[13].Set<ver::dv::Type::Position3D>({ side,side,-side });
		vertices[13].Set<ver::dv::Type::Texture2D>({ 1.0f, 0.0f });
		vertices[14].Set<ver::dv::Type::Position3D>({ -side,side,side });
		vertices[14].Set<ver::dv::Type::Texture2D>({ 0.0f, 1.0f });
		vertices[15].Set<ver::dv::Type::Position3D>({ side,side,side });
		vertices[15].Set<ver::dv::Type::Texture2D>({ 1.0f, 1.0f });


		vertices[16].Set<ver::dv::Type::Position3D>({ -side,side,-side });
		vertices[16].Set<ver::dv::Type::Texture2D>({ 0.0f, 0.0f });
		vertices[17].Set<ver::dv::Type::Position3D>({ -side,-side,-side });
		vertices[17].Set<ver::dv::Type::Texture2D>({ 0.0f, 1.0f });
		vertices[18].Set<ver::dv::Type::Position3D>({ -side,side,side });
		vertices[18].Set<ver::dv::Type::Texture2D>({ 1.0f, 0.0f });
		vertices[19].Set<ver::dv::Type::Position3D>({ -side,-side,side });
		vertices[19].Set<ver::dv::Type::Texture2D>({ 1.0f, 1.0f });

		vertices[20].Set<ver::dv::Type::Position3D>({ side,-side,-side });
		vertices[20].Set<ver::dv::Type::Texture2D>({ 0.0f, 0.0f });
		vertices[21].Set<ver::dv::Type::Position3D>({ -side,-side,-side });
		vertices[21].Set<ver::dv::Type::Texture2D>({ 1.0f, 0.0f });
		vertices[22].Set<ver::dv::Type::Position3D>({ side,-side,side });
		vertices[22].Set<ver::dv::Type::Texture2D>({ 0.0f, 1.0f });
		vertices[23].Set<ver::dv::Type::Position3D>({ -side,-side,side });
		vertices[23].Set<ver::dv::Type::Texture2D>({ 1.0f, 1.0f });

		return{
			std::move(vertices),{
				2,1,0,   1,2,3,
				4,5,6,   7,6,5,
				8,9,10,   8,10,11,
				14,13,12,   13,14,15,
				16,17,18, 19,18,17,
				22,21,20,  21,22,23
			}
		};
	}
};