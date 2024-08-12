#include <PhysX/Scene.h>
#include <PhysX/Foundation.h>
#include <thread>

using namespace physx;
PxFilterFlags FilterShaderExample(
    PxFilterObjectAttributes attributes0, PxFilterData filterData0,
    PxFilterObjectAttributes attributes1, PxFilterData filterData1,
    PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
{
    // let triggers through
    if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
        return PxFilterFlag::eDEFAULT;
    }
    // generate contacts for all that were not filtered above
    pairFlags = PxPairFlag::eCONTACT_DEFAULT;

    // trigger the contact callback for pairs (A,B) where
    // the filtermask of A contains the ID of B and vice versa.
    if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
    {
        pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND
            | PxPairFlag::ePRE_SOLVER_VELOCITY;
    }
    return PxFilterFlag::eDEFAULT;
}

ver::ph::Scene::Scene(Physics& phy, PxSimulationEventCallback* callback)
{
    auto& p = phy.get_physics();
    physx::PxSceneDesc desc{ p.getTolerancesScale() };
    desc.cpuDispatcher = &phy.get_dispatcher();
    desc.filterShader = FilterShaderExample;
    desc.simulationEventCallback = callback;
    desc.flags |= PxSceneFlag::eENABLE_PCM;
    scene.reset(p.createScene(desc));
    characters.reset(PxCreateControllerManager(*scene));
}

void ver::ph::Scene::set_gravity(physx::PxVec3 gravity)
{
    scene->setGravity(gravity);
}