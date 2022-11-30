#pragma once
#include <Game/Camera.h>
#include <Character.h>
#include <PxSceneLock.h>
#include <Util/Converter.h>
#include <Game/Entity.h>

namespace UT
{
	class Player : IEntity
	{
		static inline constexpr const float g = 9.81f;
	public:
		Player(ver::ph::Scene& scene, float radius = 0.6f, float height = 3.0f)
			:physics(scene, radius, height), height(height)
		{
			physics.get_controller().getActor()->userData = (this);
		}
	public:
		void Teleport(DirectX::XMFLOAT3 world_pos)
		{
			physx::PxSceneWriteLock scopedLock(*physics.get_controller().getScene());
			physics.get_controller().setPosition(physx::PxExtendedVec3(world_pos.x, world_pos.y, world_pos.z));
			world_pos.y += height / 3.0f;
			camera.SetPosition(world_pos);
		}
		void Update(DirectX::XMFLOAT3 accumulated_displacement, float dt)
		{
			auto&& c = physics.get_controller();
			auto t = camera.TransformToView(accumulated_displacement);

			if (!flight_mode)
			{
				vertical_velocity += g * dt * 2;
				t.y = -vertical_velocity * dt;
			}

			physx::PxSceneReadLock scopedLock(*c.getScene());
			const auto flags = c.move(convert<physx::PxVec3>(t), 0.1f, dt, {}); //for jump
			if (flags.isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN))
				vertical_velocity = g;
		}
		void Sync()
		{
			auto& position = physics.get_controller().getPosition();
			camera.SetPosition({ float(position.x), float(position.y + height / 3.0f), float(position.z) });
		}
		void Rotate(float dx, float dy)
		{
			camera.Rotate(dx, dy);
		}
		auto GetViewMatrix()const
		{
			return camera.GetViewMatrix();
		}
		auto GetPosition()const noexcept
		{
			return camera.GetPosition();
		}
		bool Flight()const noexcept
		{
			return flight_mode;
		}
		void SetFlight(bool b)noexcept
		{
			flight_mode = b;
		}
		void ToggleFlight()noexcept
		{
			flight_mode ^= true;
		}
	private:
		Camera camera;
		ver::ph::CharacterController physics;
		float height = 0.0f;
		float vertical_velocity = g;
		bool flight_mode = false;
	};
}
