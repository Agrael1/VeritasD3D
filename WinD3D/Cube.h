#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		constexpr float side = 1.0f / 2.0f;

		std::vector<V> vertices(8);
		vertices[0].pos = { -side, -side, -side }; // 0
		vertices[1].pos = {side, -side, -side}; // 1
		vertices[2].pos = {-side, side, -side}; // 2
		vertices[3].pos = {side, side, -side}; // 3
		vertices[4].pos = {-side, -side, side}; // 4
		vertices[5].pos = {side, -side, side}; // 5
		vertices[6].pos = {-side, side, side}; // 6
		vertices[7].pos = {side, side, side}; // 7

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