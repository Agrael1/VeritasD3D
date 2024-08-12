#pragma once
#include <Engine/Scene/SolidCube.h>
#include <PhysX/Foundation.h>
#include <Game/Entities/RigidBody.h>

class Wall : public PhysicalBody
{
public:
    Wall(Graphics& gfx, ver::ph::Physics& phy, float width, float height);
public:
    void AddToScene(ver::ph::Scene& scene);
    void OnContact(PhysicalBody* other, std::span<physx::PxContactPairPoint> contacts)override;

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
    DirectX::XMFLOAT3 GetPosition()const;
private:
    ver::SolidCube mesh; // Mesh component
    RigidBody<RigidBodyType::Static> physics; // Physics component
};