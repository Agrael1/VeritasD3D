#pragma once
#include <Scene/SolidSphere2.h>

class Graphics;

namespace ver
{
	struct PointLightConsts;
	class LightBuffer;

	class LightSphere
	{
	public:
		LightSphere() = default;
		LightSphere(LightBuffer& allocator, Graphics& gfx, float radius = 0.5f);
		winrt::IAsyncAction InitializeAsync(LightBuffer& allocator, Graphics& gfx, float radius = 0.5f);
	public:
		void SpawnControlWindow()noexcept;
		void Reset()noexcept;
		void Submit() const noxnd;
		void Bind(Graphics& gfx)const noexcept;
		void LinkTechniques(RG::RenderGraph& rg)
		{
			mesh.LinkTechniques(rg);
		}
		void UnlinkTechniques()
		{
			mesh.UnlinkTechniques();
		}
	private:
		mutable bool changed = false;
		PointLightConsts* cbData = nullptr;
		mutable SolidSphere mesh;
	};
}