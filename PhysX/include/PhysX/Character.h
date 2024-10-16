#pragma once
#include <PhysX/utils.h>

namespace physx
{
	class PxController;
	class PxUserControllerHitReport;
}

namespace ver::ph
{
	class Scene;
	class CharacterController
	{
	public:
		CharacterController(Scene& scene, float radius = 0.25f, float height = 1.0f, physx::PxUserControllerHitReport* report = nullptr);
	public:
		template<class Self>
		auto&& get_controller(this Self&& s)noexcept
		{
			return *s.controller;
		}
	private:
		ver::ph::physx_ptr<physx::PxController> controller;
		ver::ph::physx_ptr<physx::PxMaterial> material;
	};
}