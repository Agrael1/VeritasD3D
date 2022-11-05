#include <Foundation.h>
#include <cassert>

ver::ph::Physics::Physics()
	:foundation(PxCreateFoundation(PX_PHYSICS_VERSION, da,
		de))
{
	assert(foundation && "PxCreateFoundation failed!");
	debugger.reset(physx::PxCreatePvd(*foundation));
	auto* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	debugger->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);


	physics.reset(PxCreatePhysics(PX_PHYSICS_VERSION, *foundation,
		physx::PxTolerancesScale{}, record_allocations, debugger.get()));
	assert(physics && "Physics creation failed!");
}