#include <Game/Interaction.h>
#include <Game/Entities/PhysicalBody.h>
#include <Game/Player.h>
#include <Game/Flag.h>
#include <ranges>

using namespace physx;

void arka::Interaction::OnTrigger(std::span<physx::PxTriggerPair> pairs)
{
	for (auto& i : pairs | std::views::filter([](PxTriggerPair& p) {return p.status == physx::PxPairFlag::Enum::eNOTIFY_TOUCH_FOUND; }))
	{
	}
}

void arka::Interaction::OnContact(std::span<const physx::PxContactPair> pairs)
{
	for (auto& i : pairs)
	{
        auto* first = reinterpret_cast<PhysicalBody*>(i.shapes[0]->userData);
        auto* second = reinterpret_cast<PhysicalBody*>(i.shapes[1]->userData);

        auto points = std::make_unique<PxContactPairPoint[]>(i.contactCount);
        i.extractContacts(points.get(), i.contactCount);

        if (first && second)
        {
            first->OnContact(second, {points.get(), i.contactCount });
            second->OnContact(first, { points.get(), i.contactCount });
        }
	}
}
