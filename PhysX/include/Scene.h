#pragma once
#include <foundation/PxVec3.h>
#include <utils.h>

namespace physx
{
	class PxScene;
	class PxControllerManager;
}

namespace ver::ph
{
	class Physics;
	class Scene
	{
	public:
		Scene(Physics& phy);
	public:
		template<class Self>
		auto&& get_scene(this Self&& s)noexcept
		{
			return *s.scene;
		}
		template<class Self>
		auto&& get_char_manager(this Self&& s)noexcept
		{
			return *s.characters;
		}
		void set_gravity(physx::PxVec3 gravity);
	private:
		ver::ph::physx_ptr<physx::PxScene> scene;
		ver::ph::physx_ptr<physx::PxControllerManager> characters;
	};
}