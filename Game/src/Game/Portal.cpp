#include <Game/Portal.h>
#include <Game/Player.h>
#include <PhysX/Foundation.h>
#include <Util/Converter.h>
#include <Engine/Bindable/Light.h>
#include <imgui.h>
#include <Engine/Util/scoped_semaphore.h>

ver::IAsyncAction UT::Portal::InitializeAsync(ver::LightBuffer& lb, ver::ph::Physics& phy, Graphics& gfx, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color)
{
	co_await winrt::resume_background();
	this->position = position;
	auto mv = ver::BillboardBuilder{}.At({ position.x, position.y, position.z })
		.Color(color).Spherical().Texture(u"../models/portal3.dds")
		.Dimensions({ 5,5 }).MakeAsync(gfx, model);

	auto l = light.InitializeAsync(lb, gfx);

	auto&& p = phy.get_physics();
	//create box actor phy
	physics.reset(p.createRigidStatic(physx::PxTransform{ convert<physx::PxVec3>(position) }));

	if (!shape)
	{
		ver::scoped_semaphore lock(shape_lock);
		if (!shape)
			shape.reset(p.createShape(physx::PxBoxGeometry(2, 2, 2),
				*phy.RegisterMaterial("none", 0, 0, 0), false,
				physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eTRIGGER_SHAPE));
	}
	ref++;
	physics->attachShape(*shape);
	physics->userData = this;

	co_await l;
	co_await mv;
	light.Attributes().diffuseIntensity = 0.7f;
	light.SetColor(color);
	light.SetPosition({ position.x, position.y, position.z, 0 });
}

void UT::Portal::Teleport(UT::Player& pl)
{
	if (!bound)return;
	pl.Teleport(bound->position);
}

void UT::Portal::SpawnControlWindow()
{
	//model.SpawnControlWindow();
	//light.SpawnControlWindow();
}
