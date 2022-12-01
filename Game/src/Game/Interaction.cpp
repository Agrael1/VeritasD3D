#include <Game/Interaction.h>
#include <Game/Portal.h>
#include <Game/Player.h>
#include <Game/Flag.h>
#include <ranges>

using namespace physx;

void UT::Interaction::OnTrigger(std::span<physx::PxTriggerPair> pairs)
{
	for (auto& i : pairs | std::views::filter([](PxTriggerPair& p) {return p.status == physx::PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND; }))
	{
		auto* player = dynamic_cast<Player*>((IEntity*)i.otherActor->userData);
		if (!player)continue;

		if (auto* portal = dynamic_cast<Portal*>((IEntity*)i.triggerActor->userData))
		{
			if (!ignore)
			{
				events.emplace_back(std::make_unique<TeleportEvent>(*player, *portal));
				ignore = true;
				continue;
			}
			ignore = false;
		}
		if (auto* flag = dynamic_cast<Flag*>((IEntity*)i.triggerActor->userData))
			events.emplace_back(std::make_unique<FlagTakenEvent>(*player, *flag));
	}
}

void UT::TeleportEvent::Execute()
{
	po.Teleport(pl);
}
void UT::FlagTakenEvent::Execute()
{
	if (auto* flg = pl.GetFlag())
	{
		po.AddScore();
		flg->Taken(false);
		return pl.Reset();
	}
	pl.TakeFlag(po.GetColor(), &po);
	po.Taken(true);
}
