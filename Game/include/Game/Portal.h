#pragma once
#include <Engine/Scene/BillboardComponent.h>
#include <Scene.h>
#include <Game/Entity.h>

namespace UT
{
	class Player;
	class Portal : public IEntity
	{
		static inline std::mutex shape_lock;
		static inline ver::ph::physx_ptr<physx::PxShape> shape{};
		static inline std::atomic_uint32_t ref = 0;
	public:
		Portal() = default;
		~Portal()
		{
			if (!--ref)shape.reset();
		}
		winrt::IAsyncAction InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color);
	public:
		void SetBound(Portal* other) { bound = other; }
		void Teleport(Player& pl);
		void Submit(Graphics& gfx)
		{
			model.Submit();
			model.Update(gfx);
		}
		void AddToScene(ver::ph::Scene& sc)
		{
			sc.get_scene().addActor(*physics);
		}
		void Link(RG::RenderGraph& rg)
		{
			model.LinkTechniques(rg);
		}
		void SpawnControlWindow();
	private:
		ver::BillboardComponent model;
		ver::ph::physx_ptr<physx::PxRigidStatic> physics;
		DirectX::XMFLOAT3 position;
		Portal* bound = nullptr;
	};
}