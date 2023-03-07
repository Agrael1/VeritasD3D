#pragma once
#include <Engine/Graphics.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/Viewport.h>
#include <Engine/Util/scoped_semaphore.h>

namespace ver
{
	class Picture
	{
	public:
		Picture(Graphics& gfx) :cbuf1(gfx, 0, 0), cbuf2(gfx, 1, 0) {}
	public:
		ver::IAsyncAction InitializeAsync(Graphics& gfx)
		{
			co_await winrt::resume_background();
			auto px = ver::PixelShader::ResolveAsync(gfx, "stereo.ps.cso");
			auto vx = ver::VertexShader::ResolveAsync(gfx, "fullscreen.vs.cso");

			sam = ver::Sampler::Resolve(gfx);
			vs = co_await vx;
			ps = co_await px;
		}
		ver::IAsyncAction LoadFileAsync(Graphics& gfx, std::filesystem::path p)
		{
			auto t = std::make_unique<ver::Texture>();
			co_await t->InitializeAsync(gfx, p);
			auto [w, h] = t->Dimensions();
			auto wx2 = w * 0.5f;
			auto w2 = gfx.GetWidth() * 0.5f;
			auto hx2 = h * 0.5f;
			auto h2 = gfx.GetHeight() * 0.5f;
			auto xvp = std::make_unique<ver::Viewport>(gfx, w, h, w2 - wx2, h2 - hx2);

			{
				ver::scoped_semaphore sem{tex_lock};
				std::swap(vp, xvp);
				std::swap(texture, t);
			}
		}
	public:
		void Execute(Graphics& gfx)
		{
			ver::scoped_semaphore sem{tex_lock};
			gfx.BeginFrame(0, 0, 0);
			ver::Topology::Bind(gfx);
			sam->Bind(gfx);
			texture->Bind(gfx);
			vs->Bind(gfx);
			ps->Bind(gfx);

			gfx.GetLeftTarget()->BindAsBuffer(gfx);
			vp->Bind(gfx);
			cbuf1.Bind(gfx);
			gfx.Draw(3u);

			gfx.GetRightTarget()->BindAsBuffer(gfx);
			vp->Bind(gfx);
			cbuf2.Bind(gfx);
			gfx.Draw(3u);

			gfx.EndFrame();
		}
	private:
		std::binary_semaphore tex_lock{ 1 };
		std::unique_ptr<ver::Texture> texture;
		std::unique_ptr<ver::Viewport> vp;

		std::shared_ptr<ver::VertexShader> vs;
		std::shared_ptr<ver::PixelShader> ps;
		std::shared_ptr<ver::Sampler> sam;

		ver::PixelConstantBuffer<uint32_t> cbuf1;
		PixelConstantBuffer<uint32_t> cbuf2;
	};
}