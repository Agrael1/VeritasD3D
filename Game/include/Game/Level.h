#pragma once
#include <Game/PointLight.h>
#include <Engine/Loading/Model.h>
#include <Engine/Bindable/Light.h>
#include <Engine/Scene/BillboardComponent.h>
#include <Game/Portal.h>
#include <Scene.h>

namespace UT
{
	struct TwoWayPortal
	{
	public:
		TwoWayPortal() = default;
		winrt::IAsyncAction InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, std::pair<DirectX::XMFLOAT3, DirectX::XMFLOAT3> positions, DirectX::XMFLOAT3 color);
	public:
		void AddToScene(ver::ph::Scene& sc)
		{
			first.AddToScene(sc);
			second.AddToScene(sc);
		}
		void Submit(Graphics& gfx)
		{
			first.Submit(gfx);
			second.Submit(gfx);
		}
		void Link(RG::RenderGraph& rg)
		{
			first.Link(rg);
			second.Link(rg);
		}
		void SpawnControlWindow()
		{
			first.SpawnControlWindow();
			second.SpawnControlWindow();
		}
	private:
		Portal first;
		Portal second;
	};

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
			for (auto& i : portals)
				i.AddToScene(sc);
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
			for (auto& i : portals)
				i.Link(rg);
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
		std::array<TwoWayPortal, 6> portals;
	};
}