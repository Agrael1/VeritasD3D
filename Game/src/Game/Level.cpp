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


winrt::IAsyncAction UT::TwoWayPortal::InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> positions, DirectX::XMFLOAT3 color)
{
	co_await winrt::when_all(first.InitializeAsync(phy, gfx, positions.first, color), second.InitializeAsync(phy, gfx, positions.second, color));
	first.SetBound(&second);
	second.SetBound(&first);
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

	auto wrld = world.InitializeAsync(gfx, *pScene, std::move(map), 40.0f);
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
	}();
	auto lb = light_buf.InitializeAsync(gfx);


	constexpr std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> pos_p[]{
		{{-145.9f, -38.4f, -10.6},{-152.2f, 79.5f, -10.5f}},
		{{150.9f, -38.9f, 2.6f},{157.7f, 79.1f, 2.6f}},
		{{-154.9f, -38.6f, -35.6f},{-129.6f, 23.4f, -10.2f}},
		{{160.8f, -39.7f, 28.5f},{136.4f,22.5f,1.8f}},
		{{-139.5f,-37.5f,-36.5f},{-120.8f,2.3f,-10.0f}},
		{{144.5f,-38.8f, 28.0},{129.5f, 1.4f, 1.9f}}
	};
	constexpr DirectX::XMFLOAT3 cols_p[2]{ { 1,0,0 }, {0,0,1} };


	std::vector<winrt::IAsyncAction> acts;
	acts.reserve(portals.size() + billboards.size() + flames.size());

	for (auto& i : billboards)
		acts.push_back(i.InitializeAsync(gfx, u"../models/white.png", { 5,5 }));
	for (auto& i : flames)
		acts.push_back(i.InitializeAsync(gfx, u"../models/fire.dds", { 8,8 }, false));

	for (size_t i = 0; i < portals.size(); i++)
		acts.push_back(portals[i].InitializeAsync(phy, gfx, pos_p[i], cols_p[i % 2]));

	co_await winrt::when_all(wrld, phys, lb, ver::when_all(acts));



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
	DirectX::XMStoreFloat4A(&rpos, DirectX::XMVector3Transform(lights[0].GetPosition(), DirectX::XMMatrixRotationRollPitchYaw(std::numbers::pi / 2.f, 0, 0)));
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

	constexpr DirectX::XMFLOAT3A pos3[4]{ { -146.6f, 97.5f, -20.0f }, {-146.6f, 97.5f, 0.3f}, {153.0f, 96.7f, 11.9f}, {153.0f, 96.7f, -7.9f} };
	constexpr DirectX::XMFLOAT3A cols3[4]{ { 1,0,0 }, {1,0,0}, {0,0,1} , {0,0,1} };

	for (size_t i = 0; i < flames.size(); i++)
	{
		flames[i].SetPosition(DirectX::XMLoadFloat3A(&pos3[i]));
		flames[i].SetColor(gfx, DirectX::XMLoadFloat3A(&cols3[i]));
	}


	co_await red.InitializeAsync(phy, gfx, "../models/flag/redflag.obj", { -147.0f, -41.0f, 15.3f });
	red.GetModel()->SetRootTransform(DirectX::XMMatrixRotationY(-std::numbers::pi / 2.0f) * DirectX::XMMatrixTranslation(-147.0f, -41.0f, 15.3f));
	red.SetColor({ 1.0f, 0, 0 });
	red.SetTeamTag("Red");
	co_await blue.InitializeAsync(phy, gfx, "../models/flag/blueflag.obj", { 153.0f, -43.7f, -24.3f });
	blue.GetModel()->SetRootTransform(DirectX::XMMatrixRotationY(3.0f * std::numbers::pi / 4.0f) * DirectX::XMMatrixTranslation(153.0f, -43.7f, -24.3f));
	blue.SetColor({ 0, 0, 1 });
	blue.SetTeamTag("Blue");
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
	for (auto& i : flames)
	{
		i.Bind(gfx);
		i.Submit();
		i.Update(gfx);
	}
	for (auto& i : portals)
		i.Submit(gfx);
	red.Submit();
	blue.Submit();

	DirectX::XMFLOAT4A rpos;
	DirectX::XMStoreFloat4A(&rpos, DirectX::XMVector3Transform(pos, DirectX::XMMatrixRotationRollPitchYaw(gfx.GetFrameStep() / 4.0f, 0, 0)));
	lights[0].SetPosition(rpos);
}

void UT::Level::SpawnControlWindow()
{
	//probe.SpawnWindow(*red.GetModel());
	//probe.SpawnWindow(*blue.GetModel());
}