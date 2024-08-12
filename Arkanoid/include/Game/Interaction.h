#pragma once
#include <PxSimulationEventCallback.h>
#include <span>

namespace arka
{
    class Interaction : public physx::PxSimulationEventCallback
    {
        virtual void							onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
        {
            OnContact({ pairs, nbPairs });
        }
        virtual void							onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) { OnTrigger({ pairs, count }); }
        virtual void							onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
        virtual void							onWake(physx::PxActor**, physx::PxU32) {}
        virtual void							onSleep(physx::PxActor**, physx::PxU32) {}
        virtual void							onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}
    private:
        void OnTrigger(std::span<physx::PxTriggerPair> pairs);
        void OnContact(std::span<const physx::PxContactPair> pairs);
    };
}
