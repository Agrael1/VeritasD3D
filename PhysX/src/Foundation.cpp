#include <Foundation.h>
#include <cassert>
#include <thread>

using namespace physx;

ver::ph::Physics::Physics()
	:foundation(PxCreateFoundation(PX_PHYSICS_VERSION, da,
		de)), dispatcher{ physx::PxDefaultCpuDispatcherCreate(std::thread::hardware_concurrency()) }
{
	assert(foundation && "PxCreateFoundation failed!");
	debugger.reset(physx::PxCreatePvd(*foundation));
	auto* transport = physx::PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	debugger->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

	physics.reset(PxCreateBasePhysics(PX_PHYSICS_VERSION, *foundation,
		physx::PxTolerancesScale{}, record_allocations, debugger.get()));
	assert(physics && "Physics creation failed!");

	cooking.reset(PxCreateCooking(PX_PHYSICS_VERSION, *foundation, physics->getTolerancesScale()));
	assert(physics && "Cooking creation failed!");

	RegisterMaterial("world", 0.5f, 0.5f, 0.1f);
}

ver::ph::physx_ptr<physx::PxTriangleMesh> ver::ph::Physics::MakeTriangleMesh(std::span<DirectX::XMFLOAT3> vertices, std::span<uint16_t> indices)const
{
	PxTriangleMeshDesc meshDesc;
	meshDesc.points.count = PxU32(vertices.size());
	meshDesc.points.stride = PxU32(sizeof(PxVec3));
	meshDesc.points.data = vertices.data();

	meshDesc.triangles.count = PxU32(indices.size() / 3);
	meshDesc.triangles.stride = PxU32(3 * sizeof(uint16_t));
	meshDesc.triangles.data = indices.data();
	meshDesc.flags |= PxMeshFlag::e16_BIT_INDICES;

	PxDefaultMemoryOutputStream writeBuffer;
	PxTriangleMeshCookingResult::Enum result;
	if (!cooking->cookTriangleMesh(meshDesc, writeBuffer, &result))
		return nullptr;

	PxDefaultMemoryInputData readBuffer(writeBuffer.getData(), writeBuffer.getSize());
	return ver::ph::physx_ptr<physx::PxTriangleMesh> { physics->createTriangleMesh(readBuffer) };
}

ver::ph::physx_ptr<physx::PxRigidActor> ver::ph::Physics::MakeActor(ver::ph::physx_ptr<physx::PxTriangleMesh> shape, const physx::PxMaterial& mat, float scale)
{
	ver::ph::physx_ptr<physx::PxRigidActor> actor{ physics->createRigidStatic(physx::PxTransform{physx::PxVec3{PxZERO{}}}) };
	PxTriangleMeshGeometry triGeom{ shape.get(),physx::PxMeshScale{scale} }; //consider scaling

	ver::ph::physx_ptr<PxShape> xshape{ physics->createShape(triGeom, mat, true) };
	actor->attachShape(*xshape);
	return actor;
}

physx::PxMaterial* ver::ph::Physics::RegisterMaterial(std::string name, float staticFriction, float dynamicFriction, float restitution)
{
	auto x = materials.find(name);
	return x != materials.end() ?
		x->second.get() :
		materials.emplace(std::move(name), physics->createMaterial(staticFriction, dynamicFriction, restitution)).first->second.get();
}