#pragma once
#include <PxSimulationEventCallback.h>
#include <span>
#include <queue>

namespace UT
{
	class Player;
	class Portal;

	struct TeleportEvent {
		TeleportEvent(Player& pl, Portal& po) :pl(pl), po(po) {};
		void Execute();
		Player& pl;
		Portal& po;
	};

	class Interaction : public physx::PxSimulationEventCallback
	{
		virtual void							onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs) {}
		virtual void							onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) { OnTrigger({ pairs, count }); }
		virtual void							onConstraintBreak(physx::PxConstraintInfo*, physx::PxU32) {}
		virtual void							onWake(physx::PxActor**, physx::PxU32) {}
		virtual void							onSleep(physx::PxActor**, physx::PxU32) {}
		virtual void							onAdvance(const physx::PxRigidBody* const*, const physx::PxTransform*, const physx::PxU32) {}
	public:
		void Apply()
		{
			ignore ^= events.size() > 0;
			if (!ignore)
				for (auto& i : events)
					i.Execute();
			events.clear();
		}
	private:
		void OnTrigger(std::span<physx::PxTriggerPair> pairs);
	private:
		std::vector<TeleportEvent> events;
		bool ignore = true;
	};
}
