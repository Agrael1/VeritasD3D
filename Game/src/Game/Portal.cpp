#include <Game/Portal.h>
#include <Game/Player.h>
#include <Foundation.h>
#include <Util/Converter.h>
#include <imgui.h>

winrt::IAsyncAction UT::Portal::InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color)
{
	co_await model.InitializeAsync(gfx, u"../models/portal2.dds", { 5,5 });
	model.SetPosition(DirectX::XMLoadFloat3(&position));
	model.SetColor(gfx, DirectX::XMLoadFloat3(&color));
	this->position = position;


	auto&& p = phy.get_physics();
	//create box actor phy
	physics.reset(p.createRigidStatic(physx::PxTransform{ convert<physx::PxVec3>(position) }));

	if (!shape)
	{
		shape.reset(p.createShape(physx::PxBoxGeometry(2,2,2), 
			*phy.RegisterMaterial("none", 0,0,0),false, 
			physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eTRIGGER_SHAPE));
	}
	ref++;
	physics->attachShape(*shape);
	physics->userData = this;
}

void UT::Portal::Teleport(UT::Player& pl)
{
	if (!bound)return;
	pl.Teleport(bound->position);
}

void UT::Portal::SpawnControlWindow()
{
	model.SpawnControlWindow();
}
