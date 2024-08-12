#include <Game/Entities/Wall.h>
#include <Util/Converter.h>
#include <PhysX/Scene.h>

Wall::Wall(Graphics& gfx, ver::ph::Physics& phy, float width, float height)
    :
    mesh(gfx, width, height),
    physics(phy.get_physics(), physx::PxTransform{ convert<physx::PxVec3>(mesh.GetPos()) })
{
    auto& p = phy.get_physics();
    auto shape = p.createShape(physx::PxBoxGeometry(width / 2, 1, height / 2), *phy.RegisterMaterial("none1", 0, 0, 0), false);
    SetupFiltering(shape, +CollisionLayer::Wall, +CollisionLayer::Ball | +CollisionLayer::Paddle);
    physics->attachShape(*shape);
    physics->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
    physics.SetCallbackReciever(*shape, this);

    mesh.SetColor({ 0.0f, 0.5f, 0.0f });
    mesh.UpdateColor(gfx);
}

void Wall::AddToScene(ver::ph::Scene& scene)
{
    scene.get_scene().addActor(*physics.get_physics());
}

void Wall::OnContact(PhysicalBody* other, std::span<physx::PxContactPairPoint>)
{
    // For now do nothing
}

void Wall::SetPosition(DirectX::XMFLOAT3 pos)
{
    physics->setGlobalPose(physx::PxTransform{ convert<physx::PxVec3>(pos) });
    mesh.SetPos(pos);
}

DirectX::XMFLOAT3 Wall::GetPosition() const
{
    return mesh.GetPos();
}

