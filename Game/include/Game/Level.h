#pragma once
#include <Game/PointLight.h>
#include <Engine/Loading/Model.h>
#include <Engine/Bindable/Light.h>
#include <Engine/Scene/BillboardComponent.h>
#include <Scene.h>

namespace UT
{
	class Level
	{
	public:
		Level() = default;
		winrt::IAsyncAction InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, std::filesystem::path map);
	public:
		void Submit(Graphics& gfx);
		void AddToScene(ver::ph::Scene& sc)
		{
			sc.get_scene().addActors((physx::PxActor* const*)actors.data(), uint32_t(actors.size()));
		}
		template<class Self>
		auto&& GetWorld(this Self&& s)
		{
			return s.world;
		}
		void Link(RG::RenderGraph& rg)
		{
			world.LinkTechniques(rg);
			for (auto& i : lights)
				i.LinkTechniques(rg);
			for (auto& i : billboards)
				i.LinkTechniques(rg);
			for (auto& i : flames)
				i.LinkTechniques(rg);
		}
		void SpawnControlWindow();
	private:
		Model world;
		std::vector<ver::ph::physx_ptr<physx::PxTriangleMesh>> colliders;
		std::vector<ver::ph::physx_ptr<physx::PxRigidActor>> actors;

		ver::LightBuffer light_buf;
		std::array<ver::LightSphere, 3> lights;
		std::array<ver::BillboardComponent, 4> billboards;
		std::array<ver::BillboardComponent, 4> flames;
	};
}