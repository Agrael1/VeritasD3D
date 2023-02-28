#include <Engine/Loading/Model.h>
#include <Engine/Loading/Node.h>
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


Model::Model(Graphics& gfx, std::filesystem::path pathString, const float scale)
{
	auto path_str = pathString.string();
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(path_str.c_str(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);

	//if (pScene == nullptr || !pScene->HasMeshes())
		//throw ver::make_error<ver::ModelException>({ imp.GetErrorString() });

	Initialize(gfx, *pScene, path_str, scale);
}

winrt::Windows::Foundation::IAsyncAction Model::InitializeAsync(Graphics& gfx, const aiScene& scene, std::filesystem::path path, float scale)
{
	co_await winrt::resume_background();
	std::vector<winrt::Windows::Foundation::IAsyncAction> tasks;
	tasks.reserve(scene.mNumMeshes);
	meshPtrs.resize(scene.mNumMeshes);

	auto exec = [&](size_t i) -> winrt::Windows::Foundation::IAsyncAction {
		co_await winrt::resume_background();
		const auto& mesh = *scene.mMeshes[i];
		Material m;
		co_await m.InitializeAsync(gfx, *scene.mMaterials[mesh.mMaterialIndex], path);
		meshPtrs[i] = std::make_unique<Mesh>(gfx, m, mesh, scale);
	};

	for (size_t i = 0; i < scene.mNumMeshes; i++)
		tasks.push_back(exec(i));

	co_await ver::when_all(tasks);

	int nextId = 0;
	pRoot = ParseNode(nextId, *scene.mRootNode, scale);
	//SetRootTransform(DirectX::XMMatrixScaling(scale,scale, scale));
}
void Model::Initialize(Graphics& gfx, const aiScene& scene, std::filesystem::path path, float scale)
{
	meshPtrs.reserve(scene.mNumMeshes);
	for (size_t i = 0; i < scene.mNumMeshes; i++)
	{
		const auto& mesh = *scene.mMeshes[i];
		Material m{ gfx, *scene.mMaterials[mesh.mMaterialIndex], path };
		meshPtrs.push_back(std::make_unique<Mesh>(gfx, m, mesh, scale));
	}

	int nextId = 0;
	pRoot = ParseNode(nextId, *scene.mRootNode, scale);
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