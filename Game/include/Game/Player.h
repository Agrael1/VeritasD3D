#pragma once
#include <Game/Camera.h>
#include <Character.h>
#include <PxSceneLock.h>
#include <Util/Converter.h>

namespace UT
{
	class Player
	{
	public:
		Player(ver::ph::Scene& scene, float radius = 0.6f, float height = 3.0f)
			:physics(scene, radius, height), height(height)
		{}
	public:
		void Teleport(DirectX::XMFLOAT3 world_pos)
		{
			position = world_pos;
			OnPositionChanged();
		}
		void Update(DirectX::XMFLOAT3 accumulated_displacement, float dt)
		{
			auto&& c = physics.get_controller();
			auto t = camera.TransformToView(accumulated_displacement);
			t.y = accumulated_displacement.y;
			physx::PxSceneReadLock scopedLock(*c.getScene());
			const auto flags = c.move(convert<physx::PxVec3>(t), 0.1f, dt, {}); //for jump
		}
		void Sync()
		{
			auto& position = physics.get_controller().getPosition();
			camera.set_position({ float(position.x), float(position.y + height / 3.0f), float(position.z) });
		}
		template<class Self>
		auto&& GetCamera(this Self&& s)
		{
			return s.camera;
		}
	private:
		void OnPositionChanged()
		{
			camera.set_position({ position.x, position.y + height / 3.0f, position.z });
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
