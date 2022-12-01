#include <Game/Flag.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <Engine/Util/ModelException.h>
#include <numbers>
#include <Foundation.h>
#include <util/Converter.h>

winrt::IAsyncAction UT::Flag::InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, std::filesystem::path path, DirectX::XMFLOAT3 pos)
{
	auto graphics = [&, this]()mutable ->winrt::IAsyncAction {
		co_await winrt::resume_background();
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(path.string().data(),
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_ConvertToLeftHanded |
			aiProcess_GenNormals |
			aiProcess_CalcTangentSpace
		);
		if (pScene == nullptr || !pScene->HasMeshes())
			throw ver::make_error<ver::ModelException>({ imp.GetErrorString() });

		co_await model.InitializeAsync(gfx, *pScene, path, 5.0f);

		model.SetRootTransform(DirectX::XMMatrixRotationY(std::numbers::pi * 3.f / 4.f) * DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&pos)));
	};
	auto phys = [&, this]()mutable ->winrt::IAsyncAction {
		co_await winrt::resume_background();
		auto&& p = phy.get_physics();
		//create box actor phy
		physics.reset(p.createRigidStatic(physx::PxTransform{ convert<physx::PxVec3>(pos) }));

		if (!shape)
		{
			shape.reset(p.createShape(physx::PxBoxGeometry(2, 4, 2),
				*phy.RegisterMaterial("none", 0, 0, 0), false,
				physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eTRIGGER_SHAPE));
		}
		ref++;
		physics->attachShape(*shape);
		physics->userData = this;
	};	

	co_await winrt::when_all(graphics(), phys());
}
//148.26f, -42.168f, -23.322f
