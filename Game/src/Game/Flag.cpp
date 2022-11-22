#include <Game/Flag.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Engine/Util/ModelException.h>
#include <numbers>

winrt::IAsyncAction UT::Flag::InitializeAsync(Graphics& gfx)
{
	std::filesystem::path p{ "../models/flag/flag.obj" };
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(p.string().data(),
		aiProcess_Triangulate |
		aiProcess_JoinIdenticalVertices |
		aiProcess_ConvertToLeftHanded |
		aiProcess_GenNormals |
		aiProcess_CalcTangentSpace
	);
	if (pScene == nullptr || !pScene->HasMeshes())
		throw ver::make_error<ver::ModelException>({ imp.GetErrorString() });

	co_await model.InitializeAsync(gfx, *pScene, p, 5.0f);

	model.SetRootTransform(DirectX::XMMatrixRotationY(std::numbers::pi * 3.f / 4.f)*DirectX::XMMatrixTranslation(148.26f, -42.168f, -23.322f));
}
