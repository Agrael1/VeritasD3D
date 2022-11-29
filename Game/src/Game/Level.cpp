#include <Game/Level.h>
#include <Foundation.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Engine/Util/ModelException.h>
#include <Engine/Loading/Material.h>
#include <Engine/Graphics.h>
#include <numbers>

using namespace physx;

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
	for (auto& i : billboards)
		co_await i.InitializeAsync(gfx, u"../models/white.png", { 5,5 });

	constexpr DirectX::XMFLOAT4A pos[3]{ { -1.7f, 73.8f, 0.0f, 0.0f}, {-147.8f, -28.8f, -12.0f, 0.0f}, {154.3f, -28.8f, 0.0f, 0.0f} };
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

	DirectX::XMFLOAT4A rpos;
	DirectX::XMStoreFloat4A(&rpos, DirectX::XMVector3Transform(lights[0].GetPosition(), DirectX::XMMatrixRotationRollPitchYaw(std::numbers::pi/2.f, 0, 0)));
	lights[0].SetPosition(rpos);

	auto& l1a = lights[0].Attributes();
	l1a.diffuseIntensity = 55.f;
	l1a.attLin = 0.274f;
	l1a.attQuad = 0.001974f;

	auto& l2a = lights[1].Attributes();
	l2a.diffuseIntensity = 2.f;
	l2a.attLin = 0.086f;
	l2a.attQuad = 0.0011574f;

	auto& l3a = lights[2].Attributes();
	l3a.diffuseIntensity = 2.f;
	l3a.attLin = 0.086f;
	l3a.attQuad = 0.0011574f;

	
	constexpr DirectX::XMFLOAT3A pos2[4]{ { -115.7f, 19.9f, -0.4f }, {-115.7f, 19.9f, -20.8f}, {121.0f, 18.9f, 12.7f},{121.0f, 18.9f, -7.3f} };
	constexpr DirectX::XMFLOAT3A cols2[4]{ { 1,0,0 }, {1,0,0}, {0,0,1} , {0,0,1} };

	for (size_t i = 0; i < billboards.size(); i++)
	{
		billboards[i].SetPosition(DirectX::XMLoadFloat3A(&pos2[i]));
		billboards[i].SetColor(gfx, DirectX::XMLoadFloat3A(&cols2[i]));
	}
}

void UT::Level::Submit(Graphics& gfx)
{
	auto pos = lights[0].GetPosition();
	gfx.SetShadowCamPos(pos);
	light_buf.Bind(gfx);
	world.Submit();
	for (auto& i : billboards)
	{
		i.Bind(gfx);
		i.Submit();
	}

	DirectX::XMFLOAT4A rpos;
	DirectX::XMStoreFloat4A(&rpos, DirectX::XMVector3Transform(pos, DirectX::XMMatrixRotationRollPitchYaw(gfx.GetFrameStep()/4.0f, 0, 0)));
	lights[0].SetPosition(rpos);
}

void UT::Level::SpawnControlWindow()
{
	/*for (auto& i : billboards)
		i.SpawnControlWindow();*/
}