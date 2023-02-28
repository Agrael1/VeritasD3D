#pragma once
#include <Engine/Graphics.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/RenderTarget.h>
#include <concurrent_vector.h>

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
			textures.push_back(co_await ver::Texture::ResolveAsync(gfx, p));
		}
	public:
		void Execute(Graphics& gfx, uint32_t image)
		{
			gfx.BeginFrame(0, 0, 0);
			auto& tx = textures.at(image);

			auto [w, h] = tx->Dimensions();
			auto wx2 = w * 0.5f;
			auto w2 = gfx.GetWidth() * 0.5f;
			auto hx2 = h * 0.5f;
			auto h2 = gfx.GetHeight() * 0.5f;



			vp = std::make_shared<ver::Viewport>(gfx, w, h, w2 - wx2, h2 - hx2);

			gfx.GetLeftTarget()->BindAsBuffer(gfx);
			ver::Topology::Bind(gfx);
			vp->Bind(gfx);
			sam->Bind(gfx);
			tx->Bind(gfx);
			vs->Bind(gfx);
			ps->Bind(gfx);
			cbuf1.Bind(gfx);
			gfx.Draw(3u);

			gfx.GetRightTarget()->BindAsBuffer(gfx);
			vp->Bind(gfx);
			cbuf2.Bind(gfx);
			gfx.Draw(3u);

			gfx.EndFrame();
		}
	private:
		concurrency::concurrent_vector<std::shared_ptr<ver::Texture>> textures;
		std::shared_ptr<ver::VertexShader> vs;
		std::shared_ptr<ver::PixelShader> ps;
		std::shared_ptr<ver::Sampler> sam;
		std::shared_ptr<ver::Viewport> vp;
		ver::PixelConstantBuffer<uint32_t> cbuf1;
		PixelConstantBuffer<uint32_t> cbuf2;
	};
}