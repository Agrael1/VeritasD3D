#pragma once
#include <Engine/Scene/SolidSphere2.h>

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


		void SetPosition(DirectX::XMFLOAT4A pos);
		DirectX::XMVECTOR GetPosition();

		void SetColor(DirectX::XMFLOAT3 color);
		void LinkTechniques(RG::RenderGraph& rg)
		{
			mesh.LinkTechniques(rg);
		}

		template<class Self>
		auto& Attributes(this Self&& s)
		{
			assert(s.cbData);
			return *s.cbData;
		}
	private:
		mutable bool changed = true;
		PointLightConsts* cbData = nullptr;
		mutable ver::SolidSphere mesh;
	};
}