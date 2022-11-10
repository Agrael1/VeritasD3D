#pragma once
#include <utils.h>

namespace physx
{
	class PxController;
}

namespace ver::ph
{
	class Scene;
	class CharacterController
	{
	public:
		CharacterController(Scene& scene, float radius = 0.25f, float height = 1.0f);
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