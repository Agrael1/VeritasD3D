#pragma once
#include <utils.h>

namespace ver::ph
{
	class Physics
	{
		static constexpr bool record_allocations = true;
	public:
		Physics();
	private:
		physx::PxDefaultAllocator da;
		physx::PxDefaultErrorCallback de;
		ver::ph::physx_ptr<physx::PxFoundation> foundation;
		ver::ph::physx_ptr<physx::PxPvd> debugger;
		ver::ph::physx_ptr<physx::PxPhysics> physics;
	};
}