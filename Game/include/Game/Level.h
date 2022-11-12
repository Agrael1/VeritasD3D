#pragma once
#include <Engine/Loading/Model.h>
#include <Scene.h>

namespace UT
{
	class Level
	{
	public:
		Level(ver::ph::Physics& phy, Graphics& gfx, std::filesystem::path map);
		Level() = default;
		winrt::IAsyncAction InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, std::filesystem::path map);
	public:
		void Submit()const
		{
			world.Submit();
		}
		void AddToScene(ver::ph::Scene& sc)
		{
			sc.get_scene().addActors((physx::PxActor* const*)actors.data(), actors.size());
		}
		template<class Self>
		auto&& GetWorld(this Self&& s)
		{
			return s.world;
		}
	private:
		Model world;
		std::vector<ver::ph::physx_ptr<physx::PxTriangleMesh>> colliders;
		std::vector<ver::ph::physx_ptr<physx::PxRigidActor>> actors;
	};
}