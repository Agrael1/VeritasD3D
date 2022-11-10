#pragma once
#include <Game/Camera.h>
#include <Character.h>
#include <PxSceneLock.h>
#include <PxSceneLock.h>

namespace UT
{
	class Player
	{
	public:
		Player(ver::ph::Scene& scene, float radius = 0.25f, float height = 1.0f)
			:physics(scene, radius, height), height(height)
		{}
	public:
		void Teleport(DirectX::XMFLOAT3 world_pos)
		{
			position = world_pos;
			OnPositionChanged();
		}
		void Update()
		{
			physics.get_controller().getPosition();
		}
		template<class Self>
		auto&& GetCamera(this Self&& s)
		{
			return s.camera;
		}
	private:
		void OnPositionChanged()
		{
			camera.set_position({ position.x, position.y + height, position.z });
			physx::PxSceneWriteLock scopedLock(*physics.get_controller().getScene());
			physics.get_controller().setPosition(physx::PxExtendedVec3(position.x, position.y, position.z));
		}
	private:
		DirectX::XMFLOAT3 position{};
		Camera camera;
		ver::ph::CharacterController physics;
		float height;
	};
}
