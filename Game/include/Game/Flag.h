#pragma once
#include <Engine/Loading/Model.h>
#include <Scene.h>
#include <Game/Entity.h>

namespace UT
{
	class Flag : public IEntity
	{
		static inline ver::ph::physx_ptr<physx::PxShape> shape{};
		static inline int ref = 0;
	public:
		Flag() = default;
		~Flag() { if (!--ref)shape.reset(); }
	public:
		winrt::IAsyncAction InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, std::filesystem::path mpath, DirectX::XMFLOAT3 pos);
		void AddToScene(ver::ph::Scene& scene)
		{
			scene.get_scene().addActor(*physics);
		}
		void Submit()
		{
			if(!taken)model.Submit();
		}
		void SetColor(DirectX::XMFLOAT3 color)
		{
			this->color = color;
		}
		void SetTeamTag(std::string tag)
		{
			team_tag = tag;
		}
		std::string_view GetTeamTag()const
		{
			return team_tag;
		}
		auto GetColor()const
		{
			return color;
		}
		void Link(RG::RenderGraph& rg)
		{
			model.LinkTechniques(rg);
		}
		auto* GetModel()
		{
			return &model;
		}
		void Taken(bool t)
		{
			taken = t;
			physics->setActorFlag(physx::PxActorFlag::Enum::eDISABLE_SIMULATION, t);
		}
		void AddScore()
		{
			++score;
		}
		uint8_t GetScore()const
		{
			return score;
		}
	private:
		Model model;
		ver::ph::physx_ptr<physx::PxRigidStatic> physics;
		DirectX::XMFLOAT3 color{};
		std::string team_tag;
		bool taken = false;
		uint8_t score = 0;
	};
}