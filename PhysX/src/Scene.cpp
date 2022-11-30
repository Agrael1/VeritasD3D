#include <Scene.h>
#include <Foundation.h>
#include <thread>

using namespace physx;

ver::ph::Scene::Scene(Physics& phy, PxSimulationEventCallback* callback)
{
	auto& p = phy.get_physics();
	physx::PxSceneDesc desc{p.getTolerancesScale()};
	desc.cpuDispatcher = &phy.get_dispatcher();
	desc.filterShader = physx::PxDefaultSimulationFilterShader;
	desc.simulationEventCallback = callback;
	scene.reset(p.createScene(desc));
	characters.reset(PxCreateControllerManager(*scene));
}

void ver::ph::Scene::set_gravity(physx::PxVec3 gravity)
{
	scene->setGravity(gravity);
}