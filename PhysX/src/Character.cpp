#include <Character.h>
#include <Scene.h>
#include <numbers>

ver::ph::CharacterController::CharacterController(Scene& scene, float radius, float height)
{
	auto& phy = scene.get_scene().getPhysics();
	material.reset(phy.createMaterial(0.5f, 0.5f, 0.1f));

	physx::PxCapsuleControllerDesc desc;
	desc.radius = radius;
	desc.height = height;
	desc.slopeLimit = 0.5f;
	desc.material = material.get();
	controller.reset(scene.get_char_manager().createController(desc));
}
