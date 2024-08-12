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
        vertices.emplace_back(DirectX::XMFLOAT3{ -side, -side, -side });
        vertices.emplace_back(DirectX::XMFLOAT3{ side, -side, -side });
        vertices.emplace_back(DirectX::XMFLOAT3{ -side, side, -side });
        vertices.emplace_back(DirectX::XMFLOAT3{ side, side, -side });
        vertices.emplace_back(DirectX::XMFLOAT3{ -side, -side, side });
        vertices.emplace_back(DirectX::XMFLOAT3{ side, -side, side });
        vertices.emplace_back(DirectX::XMFLOAT3{ -side, side, side });
        vertices.emplace_back(DirectX::XMFLOAT3{ side, side, side });

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
};