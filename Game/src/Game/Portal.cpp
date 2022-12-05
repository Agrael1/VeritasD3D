#include <Game/Portal.h>
#include <Game/Player.h>
#include <Foundation.h>
#include <Util/Converter.h>
#include <imgui.h>

winrt::IAsyncAction UT::Portal::InitializeAsync(ver::ph::Physics& phy, Graphics& gfx, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color)
{
	co_await winrt::resume_background();
	this->position = position;
	auto mv = ver::BillboardBuilder{}.At({ position.x, position.y, position.z })
		.Color(color).Spherical().Texture(u"../models/portal3.dds")
		.Dimensions({ 5,5 }).MakeAsync(gfx, model);

	auto&& p = phy.get_physics();
	//create box actor phy
	physics.reset(p.createRigidStatic(physx::PxTransform{ convert<physx::PxVec3>(position) }));

	if (!ref++)
	{
		shape.reset(p.createShape(physx::PxBoxGeometry(2,2,2), 
			*phy.RegisterMaterial("none", 0,0,0),false, 
			physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eTRIGGER_SHAPE));
	}
	physics->attachShape(*shape);
	physics->userData = this;

	co_await mv;
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
