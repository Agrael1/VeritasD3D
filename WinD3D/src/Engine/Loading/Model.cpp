#include <Engine/Loading/Model.h>
#include <Engine/Loading/Node.h>
#include <Engine/Util/ModelException.h>
#include <Engine/Loading/Mesh.h>
#include <Engine/Loading/Material.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace dx = DirectX;

DirectX::XMMATRIX ScaleTranslation(DirectX::XMMATRIX matrix, float scale)
{
	dx::XMVECTOR s = _mm_load1_ps(&scale);
	s = _mm_and_ps(s, dx::g_XMMask3);
	matrix.r[3] = _mm_add_ps(_mm_mul_ps(matrix.r[3], s), dx::g_XMIdentityR3);
	return matrix;
}


Model::Model(Graphics& gfx, std::string_view pathString, const float scale)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(pathString.data(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	if (pScene == nullptr || !pScene->HasMeshes())
		throw ver::make_error<ver::ModelException>({ imp.GetErrorString() });

	//parse materials
	std::vector<Material> materials;
	materials.reserve(pScene->mNumMaterials);

	for (size_t i = 0; i< pScene->mNumMaterials; i++)
	{
		materials.emplace_back(gfx, *pScene->mMaterials[i], pathString);
	}

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		const auto& mesh = *pScene->mMeshes[i];
		meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, scale));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *pScene->mRootNode, scale);
}


winrt::Windows::Foundation::IAsyncAction
Model::MakeModelAsync(std::unique_ptr<Model>& to, Graphics& gfx, std::string_view pathString, float scale)
{
	std::unique_ptr<Model> out (new Model());

	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(pathString.data(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	auto v = imp.GetErrorString();

	if (!pScene->mNumMeshes||pScene == nullptr)
	{
		to.reset();
		co_return;
	}


	out->meshPtrs.reserve(pScene->mNumMeshes);

	std::vector<Material> materials;
	std::vector<winrt::Windows::Foundation::IAsyncAction> tasks;
	materials.resize(pScene->mNumMaterials);
	tasks.reserve(pScene->mNumMaterials);

	for (size_t i = 0; auto & mat: materials)
	{
		tasks.emplace_back(mat.MakeMaterialAsync(gfx, *pScene->mMaterials[i], pathString));
		i++;
	}
	co_await ver::when_all(tasks);

	//parse materials
	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		const auto& mesh = *pScene->mMeshes[i];
		out->meshPtrs.push_back(std::make_unique<Mesh>(gfx, materials[mesh.mMaterialIndex], mesh, scale));
	}

	int nextId = 0;
	out->pRoot = out->ParseNode(nextId, *pScene->mRootNode, scale);
	to.reset(out.release());
}



winrt::Windows::Foundation::IAsyncAction Model::InitializeAsync(Graphics& gfx, std::string_view pathString, float scale)
{
	return winrt::Windows::Foundation::IAsyncAction();
}

void Model::Submit() const noxnd
{
	pRoot->Submit(DirectX::XMMatrixIdentity());
}

void Model::SetRootTransform(DirectX::FXMMATRIX tf) noexcept
{
	pRoot->SetAppliedTransform(tf);
}

void Model::Accept(ModelProbe& probe)
{
	pRoot->Accept(probe);
}

void Model::LinkTechniques(RG::RenderGraph& rg)
{
	for (auto& pMesh : meshPtrs)
	{
		pMesh->LinkTechniques(rg);
	}
}

void Model::UnlinkTechniques()
{
	for (auto& pMesh : meshPtrs)
	{
		pMesh->UnlinkTechniques();
	}
}

Model::~Model() noexcept
{}



std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node, float scale) noexcept
{
	namespace dx = DirectX;
	const auto transform = ScaleTranslation(dx::XMMatrixTranspose(dx::XMLoadFloat4x4(
		reinterpret_cast<const dx::XMFLOAT4X4*>(&node.mTransformation)
	)), scale);

	std::vector<Mesh*> curMeshPtrs;
	curMeshPtrs.reserve(node.mNumMeshes);
	for (size_t i = 0; i < node.mNumMeshes; i++)
	{
		const auto meshIdx = node.mMeshes[i];
		curMeshPtrs.push_back(meshPtrs.at(meshIdx).get());
	}

	auto pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (size_t i = 0; i < node.mNumChildren; i++)
	{
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i], scale));
	}

	return pNode;
}