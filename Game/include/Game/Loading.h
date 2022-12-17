#pragma once
#include <Engine/Graphics.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/RenderTarget.h>

namespace UT
{
	class Loading
	{
	public:
		static winrt::IAsyncAction Execute(Graphics& gfx)
		{
			gfx.BeginFrame(0, 0, 0);
			auto tx = ver::Texture::ResolveAsync(gfx , u"../models/utload.dds");
			auto px = ver::PixelShader::ResolveAsync(gfx, "loading.ps.cso");
			auto vx = ver::VertexShader::ResolveAsync(gfx, "fullscreen.vs.cso");
			auto ss = ver::Sampler::Resolve(gfx);
			gfx.GetTarget()->BindAsBuffer(gfx);
			ver::Topology::Bind(gfx);
			(ss)->Bind(gfx);
			(co_await px)->Bind(gfx);
			(co_await vx)->Bind(gfx);
			(co_await tx)->Bind(gfx);

			gfx.Draw(3u);
			gfx.EndFrame();
		}
	};
}