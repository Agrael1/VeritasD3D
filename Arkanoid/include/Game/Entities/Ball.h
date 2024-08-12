#pragma once
#include <Engine/Scene/SolidSphere2.h>
#include <PhysX/Foundation.h>
#include <Game/Entities/RigidBody.h>

class Ball : public PhysicalBody
{
    static inline constexpr auto speed = 5.0f;
public:
    Ball(Graphics& gfx, ver::ph::Physics& phy, float radius = 0.5f);
public:
    void AddToScene(ver::ph::Scene& scene);
    void OnContact(PhysicalBody* other, std::span<physx::PxContactPairPoint> contacts)override;
    void Update();

    void Link(RG::RenderGraph& rg)
    {
        mesh.LinkTechniques(rg);
    }
    void Submit()
    {
        mesh.Submit();
    }
public:
    void SetPosition(DirectX::XMFLOAT3 pos);
    void SetLinearVelocity(DirectX::XMFLOAT2 vel);
    void SetLinearVelocity(float x, float y);
    DirectX::XMFLOAT3 GetPosition()const;
    DirectX::XMFLOAT2 GetLinearVelocity()const;
private:
    void Bounce(physx::PxVec3 collisionNormal);
private:
    ver::SolidSphere mesh; // Mesh component
    RigidBody<RigidBodyType::Dynamic> physics; // Physics component

    DirectX::XMFLOAT2 linear_velocity = { 0.0f, 0.0f };
};