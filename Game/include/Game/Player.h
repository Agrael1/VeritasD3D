#pragma once
#include <Game/Camera.h>
#include <PhysX/Character.h>
#include <PxSceneLock.h>
#include <Util/Converter.h>
#include <Game/Entity.h>
#include <Game/PointLight.h>

namespace UT
{
	class Flag;
	class Player : public IEntity
	{
		static inline constexpr const float g = 9.81f;
	public:
		Player(ver::ph::Scene& scene, ver::LightBuffer& lights, Graphics& gfx, float radius = 0.9f, float height = 3.0f)
			:physics(scene, radius, height), height(height), shimmer(lights, gfx)
		{
			physics.get_controller().getActor()->userData = (this);
			shimmer.TurnOff();
		}
	public:
		void Teleport(DirectX::XMFLOAT3 world_pos)
		{
			physx::PxSceneWriteLock scopedLock(*physics.get_controller().getScene());
			physics.get_controller().setPosition(physx::PxExtendedVec3(world_pos.x, world_pos.y, world_pos.z));
			world_pos.y += height / 3.0f;
			camera.SetPosition(world_pos);
		}
		void Jump()
		{
			if(!midair)
				vertical_velocity = -1.2f * g;
		}
		void Update(DirectX::XMFLOAT3 accumulated_displacement, float dt)
		{
			auto&& c = physics.get_controller();
			auto t = camera.TransformToView(accumulated_displacement);

			if (!flight_mode)
			{
				t.y = -vertical_velocity * dt;
				vertical_velocity += g * dt;
			}

			physx::PxSceneReadLock scopedLock(*c.getScene());
			const auto flags = c.move(convert<physx::PxVec3>(t), 0.001f, dt, {}); //for jump
			if(!(midair = !flags.isSet(physx::PxControllerCollisionFlag::eCOLLISION_DOWN)))
				vertical_velocity = g;
		}
		void Sync()
		{
			auto& position = physics.get_controller().getPosition();
			camera.SetPosition({ float(position.x), float(position.y + height / 3.0f), float(position.z) });
			if (flag)
				shimmer.SetPosition({ float(position.x) ,float(position.y + height / 3.0f), float(position.z) , 0.0f });
		}

		void TakeFlag(DirectX::XMFLOAT3 color, Flag* flg)
		{
			shimmer.SetColor(color);
			shimmer.TurnOn();
			flag = flg;
		}
		void Reset();
		void Respawn(DirectX::XMFLOAT3 pos)
		{
			Reset();
			Teleport(pos);
		}
		Flag* GetFlag()
		{
			return flag;
		}


		void Rotate(float dx, float dy)
		{
			camera.Rotate(dx, dy);
		}
		auto GetLeftViewMatrix()const
		{
			return camera.GetLeftViewMatrix();
		}
		auto GetRightViewMatrix()const
		{
			return camera.GetRightViewMatrix();
		}
		auto GetCentralCamera()const noexcept
		{
			return camera.GetCentralViewMatrix();
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
		void SpawnControlWindow()
		{
			camera.SpawnControlWindow();
		}
		bool IsMidair()const noexcept { return midair; }
		auto& GetCamera()
		{
			return camera;
		}
	private:
		StereoCamera camera;
		ver::ph::CharacterController physics;
		ver::LightSphere shimmer;
		float height = 0.0f;
		float vertical_velocity = g;
		Flag* flag = nullptr;
		bool flight_mode = false;
		bool midair = false;
	};
}
