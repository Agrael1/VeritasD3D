#pragma once
#include <Game/Entities/PhysicalBody.h>
#include <PhysX/Foundation.h>

enum class RigidBodyType
{
    Static,
    Dynamic,
    Kinematic
};

enum class CollisionLayer : physx::PxU32
{
    Ball = 1,
    Wall = 2,
    Paddle = 4
};

inline physx::PxU32 operator+(CollisionLayer l) noexcept
{
    return static_cast<physx::PxU32>(l);
}

template<RigidBodyType>
struct RBTResolver
{
    using Type = physx::PxRigidStatic; // Default is static
    static inline constexpr physx::PxRigidBodyFlag::Enum flags = {};
    static inline constexpr auto create = &physx::PxPhysics::createRigidStatic;
};
template<>
struct RBTResolver<RigidBodyType::Dynamic>
{
    using Type = physx::PxRigidDynamic;
    static inline constexpr physx::PxRigidBodyFlag::Enum flags = {};
    static inline constexpr auto create = &physx::PxPhysics::createRigidDynamic;
};
template<>
struct RBTResolver<RigidBodyType::Kinematic>
{
    using Type = physx::PxRigidDynamic;
    static inline constexpr physx::PxRigidBodyFlag::Enum flags = physx::PxRigidBodyFlag::eKINEMATIC;
    static inline constexpr auto create = &physx::PxPhysics::createRigidDynamic;
};

template<RigidBodyType type>
class RigidBody
{
    using PhysicsType = typename RBTResolver<type>::Type;
public:
    RigidBody() = default;
    RigidBody(physx::PxPhysics& physics, physx::PxTransform transform)
    {
        this->physics.reset((physics.*RBTResolver<type>::create)(transform));
        
        if constexpr (type == RigidBodyType::Kinematic)
            this->physics->setRigidBodyFlag(RBTResolver<type>::flags, true);
    }
public:
    void SetCallbackReciever(physx::PxShape& shape, PhysicalBody* body)
    {
        shape.userData = body;
    }
public:
    decltype(auto) get_physics()const { return physics.get(); }
    decltype(auto) get_physics() { return physics.get(); }
    decltype(auto) operator->()const { return get_physics(); }
    decltype(auto) operator->() { return get_physics(); }
protected:
    ver::ph::physx_ptr<PhysicsType> physics; // Physics component
};

inline void SetupFiltering(physx::PxShape* shape, physx::PxU32 filterGroup, physx::PxU32 filterMask)
{
    physx::PxFilterData filterData;
    filterData.word0 = filterGroup; // word0 = own ID
    filterData.word1 = filterMask;  // word1 = ID mask to filter pairs that trigger a contact callback
    shape->setSimulationFilterData(filterData);
    shape->setContactOffset(0.02f);
}