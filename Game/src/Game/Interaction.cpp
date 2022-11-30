#include <Game/Interaction.h>
#include <Game/Portal.h>
#include <Game/Player.h>
#include <ranges>

using namespace physx;

void UT::Interaction::OnTrigger(std::span<physx::PxTriggerPair> pairs)
{
	for (auto& i : pairs | std::views::filter([](PxTriggerPair& p) {return p.status == physx::PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND; }))
	{
		auto* portal = (Portal*)i.triggerActor->userData;
		auto* player = (Player*)i.otherActor->userData;

		if (portal && player)
			events.emplace_back(*player, *portal);
	}
}

void UT::TeleportEvent::Execute()
{
	po.Teleport(pl);
}
