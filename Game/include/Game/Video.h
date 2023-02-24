#pragma once
#include <Engine/Graphics.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/RenderTarget.h>

namespace ver
{
	class Video : public GraphicsResource
	{
	public:
		Video(Graphics& gfx) :cbuf1(gfx, 0, 0), cbuf2(gfx, 1, 0) {}
	public:
		winrt::IAsyncAction InitializeAsync(Graphics& gfx, uint32_t height, uint32_t width)
		{
			co_await winrt::resume_background();
			//surface = xsurface;
			auto px = ver::PixelShader::ResolveAsync(gfx, "videov.ps.cso");
			auto vx = ver::VertexShader::ResolveAsync(gfx, "fullscreen.vs.cso");

			rtv = std::make_shared<ShaderInputRenderTarget>(gfx, width, height, 0);

			sam = ver::Sampler::Resolve(gfx);
			vs = co_await vx;
			ps = co_await px;
		}
		auto SRV()
		{
			return rtv->SRV();
		}
		void Execute(Graphics& gfx)
		{
			gfx.BeginFrame(0, 0, 0);

			gfx.GetLeftTarget()->BindAsBuffer(gfx);
			ver::Topology::Bind(gfx);
			sam->Bind(gfx);

			rtv->BindTo(gfx, 0);

			vs->Bind(gfx);
			ps->Bind(gfx);
			cbuf1.Bind(gfx);
			gfx.Draw(3u);

			gfx.GetRightTarget()->BindAsBuffer(gfx);
			cbuf2.Bind(gfx);
			gfx.Draw(3u);

			gfx.EndFrame();
		}
	private:
		//winrt::com_ptr<ID3D11ShaderResourceView> surface;
		std::shared_ptr<ver::VertexShader> vs;
		std::shared_ptr<ver::PixelShader> ps;
		std::shared_ptr<ver::Sampler> sam;
		ver::PixelConstantBuffer<uint32_t> cbuf1;
		PixelConstantBuffer<uint32_t> cbuf2;
		std::shared_ptr <ShaderInputRenderTarget> rtv;
	};
}