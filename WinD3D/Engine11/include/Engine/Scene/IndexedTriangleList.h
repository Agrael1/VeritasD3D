#pragma once
#include <Engine/Dynamic/VertexBuffer.h>
#include <DirectXMath.h>

class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(ver::dv::VertexBuffer verts, std::vector<unsigned short> inds)
		:
		vertices(std::move(verts)),
		indices(std::move(inds))
	{
		assert(vertices.count() > 2);
		//assert(indices.size() % 3 == 0);
	}
public:
	void Deform(DirectX::FXMMATRIX matrix)
	{
		using Type = ver::dv::ElementType;
		for (int i = 0; i < vertices.count(); i++)
		{
			auto& pos = vertices[i].attribute<Type::Position3D>();
			DirectX::XMStoreFloat3(
				&pos,
				DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&pos), matrix)
			);
		}
	}
	void CalcNormalsIndependentFlat()noxnd
	{
		using namespace DirectX;
		using Type = ver::dv::ElementType;
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			auto v0 = vertices[indices[i]];
			auto v1 = vertices[indices[i + 1]];
			auto v2 = vertices[indices[i + 2]];
			const auto p0 = XMLoadFloat3(&v0.attribute<Type::Position3D>());
			const auto p1 = XMLoadFloat3(&v1.attribute<Type::Position3D>());
			const auto p2 = XMLoadFloat3(&v2.attribute<Type::Position3D>());

			const auto n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

			XMStoreFloat3(&v0.attribute<Type::Normal>(), n);
			XMStoreFloat3(&v1.attribute<Type::Normal>(), n);
			XMStoreFloat3(&v2.attribute<Type::Normal>(), n);
		}
	}

public:
	ver::dv::VertexLayout vl;
	ver::dv::VertexBuffer vertices;
	std::vector<unsigned short> indices;
};