#pragma once
#include "IndexedTriangleList.h"
#include <DirectXMath.h>
#include "EngineBase.h"

class Icosphere
{
public:
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		const float t = (1.0f + std::sqrtf(5.0f)) / 2.0f;

		std::vector<DirectX::XMFLOAT3> vertices;
		vertices.emplace_back(-1.0f, t, 0.0f); // 0
		vertices.emplace_back(1.0f, t, 0.0f); // 1
		vertices.emplace_back(-1.0f, -t, 0.0f); // 2
		vertices.emplace_back(1.0f, -t, -0.0f); // 3

		vertices.emplace_back(0.0f, -1.0f, t); // 4
		vertices.emplace_back(0.0f, 1.0f, t); // 5
		vertices.emplace_back(0.0f, -1.0f, -t); // 6
		vertices.emplace_back(0.0f, 1.0f, -t); // 7

		vertices.emplace_back(t, 0.0f, -1.0f); // 8
		vertices.emplace_back(t, 0.0f, 1.0f); // 9
		vertices.emplace_back(-t, 0.0f, -1.0f); // 10
		vertices.emplace_back(-t, 0.0f, 1.0f); // 11

		std::vector<V> verts(vertices.size());
		for (size_t i = 0; i < vertices.size(); i++)
		{
			verts[i].pos = vertices[i];
		}
		return{
			std::move(verts),{
				0, 11,5,	0, 5, 1,	0, 1, 7,	0, 7,10,	0,10,11,
				1, 5, 9,	5, 11,4,	11,10,2,	10,7, 6,	7, 1, 8,
				3, 9, 4,	3, 4, 2,	3, 2, 6,	3, 6, 8,	3, 8, 9,
				4, 9, 5,	2, 4,11,	6, 2,10,	8, 6, 7,	9, 8, 1
			}
		};
	}
};