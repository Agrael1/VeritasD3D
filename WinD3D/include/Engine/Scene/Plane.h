#pragma once
#include "IndexedTriangleList.h"
#include <optional>
#include <array>

class Plane
{
public:
	static IndexedTriangleList MakeTesselated(unsigned divX, unsigned divY, std::optional<ver::dv::VertexLayout> layout = std::nullopt)
	{
		assert(divX > 0);
		assert(divY > 0);
		constexpr float width = 2.0f;
		constexpr float height = 2.0f;

		// number of verticies
		const unsigned nVertX = divX + 1;
		const unsigned nVertY = divY + 1;

		// place in middle
		const float side_x = width / 2.0f;
		const float side_y = height / 2.0f;

		// start point with delta
		const float divisionSize_x = width / float(divX);
		const float divisionSize_y = height / float(divY);
		const auto bottomLeft = DirectX::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

		if (!layout)
		{
			layout.emplace(ver::dv::ElementType::Position3D);
		}
		DirectX::XMFLOAT3 xx;
		ver::dv::VertexBuffer vertices(std::move(*layout), size_t(nVertX * nVertY));
		{
			for (unsigned y = 0, i = 0; y < nVertY; y++)
			{
				const float y_pos = float(y) * divisionSize_y;
				for (unsigned x = 0; x < nVertX; x++, i++)
				{
					const auto v = DirectX::XMVectorAdd(
						bottomLeft,
						DirectX::XMVectorSet(float(x) * divisionSize_x, y_pos, 0.0f, 0.0f)
					);

					DirectX::XMStoreFloat3(&xx, v);
					vertices.emplace_back(xx);
				}
			}
		}

		//arrange Indicies
		std::vector<unsigned short> indices(divX * divY * divX * divY * 6);
		{
			const auto vxy2i = [nVertX](size_t x, size_t y)
			{
				return (unsigned short)(y * nVertX + x);
			};
			for (size_t y = 0; y < divY; y++)
			{
				for (size_t x = 0; x < divX; x++)
				{
					const std::array<unsigned short, 4> indexArray =
					{ vxy2i(x,y),vxy2i(x + 1,y),vxy2i(x,y + 1),vxy2i(x + 1,y + 1) };
					indices.push_back(indexArray[0]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[1]);
					indices.push_back(indexArray[2]);
					indices.push_back(indexArray[3]);
				}
			}
		}
		return{ std::move(vertices), std::move(indices) };
	}
	static IndexedTriangleList MakeTesselatedLined(unsigned divX, unsigned divY, std::optional<ver::dv::VertexLayout> layout = std::nullopt)
	{
		assert(divX > 0);
		assert(divY > 0);
		constexpr float width = 2.0f;
		constexpr float height = 2.0f;

		// number of verticies
		const unsigned nVertX = divX + 1;
		const unsigned nVertY = divY + 1;

		// place in middle
		const float side_x = width / 2.0f;
		const float side_y = height / 2.0f;

		// start point with delta
		const float divisionSize_x = width / float(divX);
		const float divisionSize_y = height / float(divY);
		const auto bottomLeft = DirectX::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);
		DirectX::XMFLOAT3 xx;

		if (!layout)
		{
			layout.emplace(ver::dv::ElementType::Position3D);
		}
		ver::dv::VertexBuffer vertices(std::move(*layout), size_t(nVertX + nVertY) * 2);
		{
			for (unsigned x = 0; x < 2 * nVertX; x += 2)
			{
				const float x_pos = float(x) * divisionSize_x / 2;

				const auto v = DirectX::XMVectorAdd(
					bottomLeft,
					DirectX::XMVectorSet(x_pos, 0.0f, 0.0f, 0.0f)
				);
				DirectX::XMStoreFloat3(&xx, v);
				vertices.emplace_back(xx);

				const auto v2 = DirectX::XMVectorAdd(
					bottomLeft,
					DirectX::XMVectorSet(x_pos, height, 0.0f, 0.0f)
				);
				DirectX::XMStoreFloat3(&xx, v2);
				vertices.emplace_back(xx);
			}
			for (unsigned y = 0; y < 2 * nVertY; y += 2)
			{
				const float y_pos = float(y) * divisionSize_x / 2;
				const auto v = DirectX::XMVectorAdd(
					bottomLeft,
					DirectX::XMVectorSet(0.0f, y_pos, 0.0f, 0.0f)
				);
				DirectX::XMStoreFloat3(&xx, v);
				vertices.emplace_back(xx);

				const auto v2 = DirectX::XMVectorAdd(
					bottomLeft,
					DirectX::XMVectorSet(width, y_pos, 0.0f, 0.0f)
				);
				DirectX::XMStoreFloat3(&xx, v2);
				vertices.emplace_back(xx);
			}
		}

		//arrange Indicies
		std::vector<unsigned short> indices;
		indices.reserve(nVertX + nVertY);

		{
			for (size_t x = 0; x < nVertX + nVertY; x++)
			{
				if (!(x & 1))
				{
					indices.push_back(x * 2);
					indices.push_back(x * 2 + 1);
				}
				else
				{
					indices.push_back(x * 2 + 1);
					indices.push_back(x * 2);
				}

			}
		}
		return{ std::move(vertices), std::move(indices) };
	}
	static IndexedTriangleList Make(std::optional<ver::dv::VertexLayout> layout = std::nullopt)
	{
		return MakeTesselated(1,1, layout);
	}
};