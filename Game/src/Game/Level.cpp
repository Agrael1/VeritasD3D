#include <Game/Level.h>
#include <Foundation.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Engine/Util/ModelException.h>
#include <Engine/Loading/Material.h>
#include <Engine/Graphics.h>

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

	co_await winrt::when_all(world.InitializeAsync(gfx, *pScene, std::move(map), 40.0f),
		phys(), light_buf.InitializeAsync(gfx));

	constexpr DirectX::XMFLOAT4A pos[3]{ { -1.7f, 14.8f, 0.0f, 0.0f}, {-152.8f, -28.8f, 0.0f, 0.0f}, {143.3f, -28.8f, 0.0f, 0.0f} };
	constexpr DirectX::XMFLOAT3 cols[3]{ { 154.f / 255.f, 154.f / 255.f, 154.f / 255.f}
	, {255.f / 255.f, 111.f / 255.f, 111.f / 255.f}
	, {134.f / 255.f, 169.f / 255.f, 255.f / 255.f} };

	for (size_t i = 0; i < 3; i++)
	{
		auto& l = lights[i];
		co_await l.InitializeAsync(light_buf, gfx);
		l.SetPosition(pos[i]);
		l.SetColor(cols[i]);
	}	
}

void UT::Level::Submit(Graphics& gfx)
{
	gfx.SetShadowCamPos(lights[0].GetPosition());
	for (auto& i : lights)
	{
		i.Bind(gfx);
		i.Submit();
	}
	light_buf.Bind(gfx);
	world.Submit();
}

void UT::Level::SpawnControlWindow()
{
	for (size_t i = 0; i < 3u; i++)
	{
		lights[i].SpawnControlWindow();
	}
}