#include "Model.h"
#include "BindableBase.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(Graphics& gfx, std::mt19937& rng,
	std::uniform_real_distribution<float>& adist,
	std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist,
	std::uniform_real_distribution<float>& rdist,
	DirectX::XMFLOAT3 material,
	float scale)
	:
	TestObject(gfx, rng, adist, ddist, odist, rdist)
{
	namespace dx = DirectX;

	if (!IsStaticInitialized())
	{
		DV::VertexBuffer vertices(std::move(
			DV::VertexLayout{}
			+ DV::VertexLayout::Position3D
			+ DV::VertexLayout::Normal
		));

		Assimp::Importer imp;
		const auto pModel = imp.ReadFile("models\\suzanne.obj",
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices
		);
		const auto pMesh = pModel->mMeshes[0];

		for (unsigned int i = 0; i < pMesh->mNumVertices; i++)
		{
			vertices.EmplaceBack(
				dx::XMFLOAT3{ pMesh->mVertices[i].x * scale,pMesh->mVertices[i].y * scale,pMesh->mVertices[i].z * scale },
				*reinterpret_cast<dx::XMFLOAT3*>(&pMesh->mNormals[i])
			);
		}

		std::vector<unsigned short> indices;
		indices.reserve(pMesh->mNumFaces * 3);
		for (unsigned int i = 0; i < pMesh->mNumFaces; i++)
		{
			const auto& face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		AddStaticBind(std::make_unique<VertexBuffer>(gfx, vertices));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, indices));

		auto pvs = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");
		auto pvsbc = pvs->GetBytecode();
		AddStaticBind(std::move(pvs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));

		AddStaticBind(std::make_unique<InputLayout>(gfx, vertices.GetLayout().GetD3DLayout(), pvsbc));

		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

		struct PSMaterialConstant
		{
			DirectX::XMFLOAT3 color;
			float specularIntensity = 0.6f;
			float specularPower = 30.0f;
			float padding[3];
		} pmc;
		pmc.color = material;
		AddStaticBind(std::make_unique<PixelConstantBuffer<PSMaterialConstant>>(gfx, pmc, 1u));
	}
	else
	{
		SetIndexFromStatic();
	}

	AddBind(std::make_unique<TransformCbuf>(gfx, *this));
}