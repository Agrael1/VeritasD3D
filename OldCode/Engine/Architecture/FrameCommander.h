#pragma once
#include <array>
#include "BindableCommons.h"
#include <Engine/Graphics.h>
#include "Job.h"
#include "Pass.h"
#include "DepthStencil.h"
#include "RenderTarget.h"
#include "NullPixelShader.h"
#include <Engine\Entities\BlurPack.h>
#include <array>

class FrameCommander
{
public:
	FrameCommander(Graphics& gfx)
		:
		ds(gfx, gfx.GetWidth(), gfx.GetHeight()),
		rt1({ gfx,gfx.GetWidth() / downFactor,gfx.GetHeight() / downFactor }),
		rt2({ gfx,gfx.GetWidth() / downFactor,gfx.GetHeight() / downFactor }),
		blur(gfx, 7, 2.6f, "BlurOutline_PS.cso")
	{
		namespace dx = DirectX;

		// setup fullscreen geometry
		DV::VertexLayout lay;
		lay
			+DV::Type::Position2D;
		DV::VertexBuffer bufFull{ lay };
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ -1,-1 });
		bufFull.EmplaceBack(dx::XMFLOAT2{ 1,-1 });
		pVbFull = VertexBuffer::Resolve(gfx, "$Full", std::move(bufFull));
		std::vector<unsigned short> indices = { 0,1,2,1,3,2 };
		pIbFull = IndexBuffer::Resolve(gfx, "$Full", std::move(indices));

		// setup fullscreen shaders
		pVsFull = VertexShader::Resolve(gfx, "Fullscreen_VS.cso");
		pLayoutFull = InputLayout::Resolve(gfx, lay, pVsFull->GetBytecode());
		pSamplerFullPoint = Sampler::Resolve(gfx, false, true);
		pSamplerFullBilin = Sampler::Resolve(gfx, true, true);
		pBlenderMerge = BlendState::Resolve(gfx, true);
	}
	void Accept(Job job, size_t target) noexcept
	{
		passes[target].Accept(job);
	}
	void Execute(Graphics& gfx) noxnd
	{
		// normally this would be a loop with each pass defining its setup / etc.
		// and later on it would be a complex graph with parallel execution contingent
		// on input / output requirements

		// setup render target used for normal passes
		ds.Clear(gfx);
		rt1->Clear(gfx);
		gfx.BindSwapBuffer(ds);
		// main phong lighting pass
		BlendState::Resolve(gfx, false)->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[0].Execute(gfx);
		// outline masking pass
		Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
		NullPixelShader::Resolve(gfx)->Bind(gfx);
		passes[1].Execute(gfx);
		// outline drawing pass
		rt1->BindAsTarget(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
		passes[2].Execute(gfx);
		// fullscreen blur h-pass
		rt2->BindAsTarget(gfx);
		rt1->BindAsTexture(gfx, 0);
		pVbFull->Bind(gfx);
		pIbFull->Bind(gfx);
		pVsFull->Bind(gfx);
		pLayoutFull->Bind(gfx);
		pSamplerFullPoint->Bind(gfx);
		blur.Bind(gfx);
		blur.SetHorizontal(gfx);
		gfx.DrawIndexed(pIbFull->GetCount());
		// fullscreen blur v-pass + combine
		gfx.BindSwapBuffer(ds);
		rt2->BindAsTexture(gfx, 0u);
		pBlenderMerge->Bind(gfx);
		pSamplerFullBilin->Bind(gfx);
		Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
		blur.SetVertical(gfx);
		gfx.DrawIndexed(pIbFull->GetCount());
	}
	void Reset() noexcept
	{
		for (auto& p : passes)
		{
			p.Reset();
		}
	}
	void ShowWindows(Graphics& gfx)
	{
		if (ImGui::Begin("Blur"))
		{
			if (ImGui::SliderInt("Down Factor", &downFactor, 1, 16))
			{
				rt1.emplace(gfx, gfx.GetWidth() / downFactor, gfx.GetHeight() / downFactor);
				rt2.emplace(gfx, gfx.GetWidth() / downFactor, gfx.GetHeight() / downFactor);
			}
			blur.RenderWidgets(gfx);
		}
		ImGui::End();
	}
private:
	std::array<Pass, 3> passes;
	int downFactor = 1;
	DepthStencil ds;
	std::optional<RenderTarget> rt1;
	std::optional<RenderTarget> rt2;
	BlurPack blur;
	std::shared_ptr<VertexBuffer> pVbFull;
	std::shared_ptr<IndexBuffer> pIbFull;
	std::shared_ptr<VertexShader> pVsFull;
	std::shared_ptr<InputLayout> pLayoutFull;
	std::shared_ptr<Sampler> pSamplerFullPoint;
	std::shared_ptr<Sampler> pSamplerFullBilin;
	std::shared_ptr<BlendState> pBlenderMerge;
};