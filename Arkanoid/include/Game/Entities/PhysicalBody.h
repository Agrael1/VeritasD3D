#pragma once
#include <span>

namespace ver::ph
{
    class Scene;
}

namespace physx
{
    class PxContactPairPoint;
}

class PhysicalBody
{
public:
    virtual ~PhysicalBody() = default;
    virtual void OnContact(PhysicalBody* other, std::span<physx::PxContactPairPoint> contacts) = 0;
};