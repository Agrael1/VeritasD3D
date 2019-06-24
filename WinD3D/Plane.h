#pragma once
#include "IndexedTriangleList.h"
#include "EngineBase.h"
#include <array>

class Plane
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(dword divX, dword divY)
	{
		assert(divX > 0);
		assert(divY > 0);
		constexpr float width = 2.0f;
		constexpr float height = 2.0f;

		// number of verticies
		const dword nVertX = divX + 1;
		const dword nVertY = divY + 1;

		// place in middle
		const float side_x = width / 2.0f;
		const float side_y = height / 2.0f;

		// start point with delta
		const float divisionSize_x = width / float(divX);
		const float divisionSize_y = height / float(divY);
		const auto bottomLeft = DirectX::XMVectorSet(-side_x, -side_y, 0.0f, 0.0f);

		// arrange Verticies
		std::vector<V> vertices(nVertX*nVertY);
		{
			for (dword y = 0, i = 0; y < nVertY; y++)
			{
				const float y_pos = float(y) * divisionSize_y;
				for (dword x = 0; x < nVertX; x++, i++)
				{
					const auto v = DirectX::XMVectorAdd(
						bottomLeft,
						DirectX::XMVectorSet(float(x) * divisionSize_x, y_pos, 0.0f, 0.0f)
					);
					DirectX::XMStoreFloat3(&vertices[i].pos, v);
				}
			}
		}

		//arrange Indicies
		std::vector<word> indices(divX * divY * divX * divY * 6);
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
		return{ std::move(vertices),std::move(indices) };
	}
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(1,1);
	}
};