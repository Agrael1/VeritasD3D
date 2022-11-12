#pragma once
#include <assimp/mesh.h>
#include <vector>
#include <concepts>


namespace ver
{
	template<std::unsigned_integral T>
	inline std::vector<T> ExtractIndices(const aiMesh& mesh)
	{
		std::vector<T> indices;
		indices.reserve(mesh.mNumFaces * 3);
		for (size_t i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(T(face.mIndices[0]));
			indices.push_back(T(face.mIndices[1]));
			indices.push_back(T(face.mIndices[2]));
		}
		return indices;
	}
}