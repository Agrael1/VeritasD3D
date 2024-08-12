#include <Game/Entities/Ball.h>
#include <Util/Converter.h>
#include <PhysX/Scene.h>

Ball::Ball(Graphics& gfx, ver::ph::Physics& phy, float radius)
    : 
    mesh(gfx, radius), 
    physics(phy.get_physics(), physx::PxTransform{ convert<physx::PxVec3>(mesh.GetPos()) })
{
    SetPosition({0,0,0});


    auto& p = phy.get_physics();
    auto shape = p.createShape(physx::PxSphereGeometry(radius), *phy.RegisterMaterial("none", 0, 0, 0), false);
    SetupFiltering(shape, +CollisionLayer::Ball, +CollisionLayer::Wall | +CollisionLayer::Paddle);

    physics->attachShape(*shape);
    physics->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
    physics.SetCallbackReciever(*shape ,this);
}

void Ball::AddToScene(ver::ph::Scene& scene)
{
    scene.get_scene().addActor(*physics.get_physics());
}

void Ball::OnContact(PhysicalBody* other, std::span<physx::PxContactPairPoint> point)
{
    if (point.size() == 0)
        return;
    Bounce(point[0].normal);
}

void Ball::Bounce(physx::PxVec3 collisionNormal)
{
    using namespace DirectX;
    auto lastFrameVelocity = physics->getLinearVelocity().getNormalized();

    XMFLOAT3 v1 = convert<XMFLOAT3>(lastFrameVelocity);
    XMFLOAT3 v2 = convert<XMFLOAT3>(collisionNormal);

    auto direction = 
        DirectX::XMVector3Reflect
        (
            DirectX::XMLoadFloat3(&v1),
            DirectX::XMLoadFloat3(&v2)
        );
    auto vec = direction * speed;
    XMFLOAT3 dir;
    DirectX::XMStoreFloat3(&dir, vec);

    physics->setLinearVelocity(convert<physx::PxVec3>(dir));
}

void Ball::Update()
{
    auto trsf = physics->getGlobalPose();
    auto& pos = trsf.p;
    mesh.SetPos({ pos.x, pos.y, pos.z });
}

void Ball::SetPosition(DirectX::XMFLOAT3 pos)
{
    physics->setGlobalPose(physx::PxTransform{ convert<physx::PxVec3>(pos) });
    mesh.SetPos(pos);
}

void Ball::SetLinearVelocity(DirectX::XMFLOAT2 vel)
{
    physics->setLinearVelocity({ vel.x, 0.0f, vel.y });
    linear_velocity = vel;
}

void Ball::SetLinearVelocity(float x, float y)
{
    SetLinearVelocity({ x, y });
}

DirectX::XMFLOAT3 Ball::GetPosition() const
{
    return mesh.GetPos();
}

DirectX::XMFLOAT2 Ball::GetLinearVelocity() const
{
    return linear_velocity;
}

