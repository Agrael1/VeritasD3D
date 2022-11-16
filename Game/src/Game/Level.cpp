#include <Game/Level.h>
#include <Foundation.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Engine/Util/ModelException.h>
#include <Engine/Loading/Material.h>

using namespace physx;

UT::Level::Level(ver::ph::Physics& phy, Graphics& gfx, std::filesystem::path map)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(map.string().data(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	if (pScene == nullptr || !pScene->HasMeshes())
		throw ver::make_error<ver::ModelException>({ imp.GetErrorString() });

	world.Initialize(gfx, *pScene, std::move(map), 40.0f);

	auto* mat = phy.GetMaterial("world");
	assert(mat);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
	{
		auto mesh = pScene->mMeshes[i];
		auto y = Material::ExtractIndices(*mesh);
		auto x = phy.MakeTriangleMesh({ (DirectX::XMFLOAT3*)mesh->mVertices, mesh->mNumVertices }, y);
		actors.emplace_back(phy.MakeActor(std::move(x), *mat, 40.0f));
	}
}

winrt::IAsyncAction UT::Level::InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, std::filesystem::path map)
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(map.string().data(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	if (pScene == nullptr || !pScene->HasMeshes())
		throw ver::make_error<ver::ModelException>({ imp.GetErrorString() });

	auto phys = [&]() ->winrt::IAsyncAction {
		co_await winrt::resume_background();
		auto* mat = phy.GetMaterial("world");
		assert(mat);

		for (size_t i = 0; i < pScene->mNumMeshes; i++)
		{
			auto mesh = pScene->mMeshes[i];
			auto y = Material::ExtractIndices(*mesh);
			auto x = phy.MakeTriangleMesh({ (DirectX::XMFLOAT3*)mesh->mVertices, mesh->mNumVertices }, y);
			actors.emplace_back(phy.MakeActor(std::move(x), *mat, 40.0f));
		}
	};
	co_await winrt::when_all(world.InitializeAsync(gfx, *pScene, std::move(map), 40.0f), phys());
}