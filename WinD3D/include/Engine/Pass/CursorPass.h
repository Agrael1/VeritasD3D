#pragma once
#include <Engine/Pass/RenderQueuePass.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/CursorTransform.h>
#include <Engine/Graphics.h>
#include <Engine/Rendering/SourcesT.h>

class Graphics;

using namespace RG;

namespace ver::rg
{
	class CursorPass : public RenderQueuePass, public GraphicsResource
	{
		struct B { float mesh; };
		static inline constexpr auto cursor_slot = 3;
	public:
		CursorPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
			, region(gfx, DXGI_FORMAT_R32G32_FLOAT, 1,1, D3D11_CPU_ACCESS_READ)
		{
			//capture depth and mesh index (interpreted)
			renderTarget = std::make_shared<ShaderInputRenderTarget>(gfx, gfx.GetWidth(), gfx.GetHeight(), 0, DXGI_FORMAT_R32G32_FLOAT);
			pCursorBuffer = std::make_shared<PixelConstantBuffer<B>>(gfx, cursor_slot);
			RegisterSink(DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(DirectBindableSource<PixelConstantBuffer<B>>::Make("cursor", pCursorBuffer));

			AddBind(VertexShader::Resolve(gfx, "shadow.vs.cso"));
			AddBind(PixelShader::Resolve(gfx, "cursor.ps.cso"));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
			AddBind(BlendState::Resolve(gfx, false));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			using namespace DirectX;
			RenderQueuePass::Execute(gfx);

			//extract coord
			auto res = ((ShaderInputRenderTarget*)renderTarget.get())->Resource();
			auto dstres = region.Resource();
			auto [x, y] = gfx.GetCursor();
			x = (x >= 0) * x;
			y = (y >= 0) * y;

			auto [w, h] = std::pair{ float(gfx.GetWidth()),float(gfx.GetHeight()) };

			auto xn = float(x) / w;
			auto yn = float(y) / h;

			D3D11_BOX box{
				.left = UINT(x),
				.top = UINT(y),
				.front = 0,
				.right = UINT(x + 1),
				.bottom = UINT(y + 1),
				.back = 1,
			};

			GetContext(gfx)->CopySubresourceRegion(dstres.get(), 0, 0, 0, 0, res.get(), 0, &box);
			check_context();

			D3D11_MAPPED_SUBRESOURCE sr{};
			ver::check_hresult(GetContext(gfx)->Map(dstres.get(), 0, D3D11_MAP_READ, 0, &sr));
			auto mm = *reinterpret_cast<DirectX::XMFLOAT2A*>(sr.pData);
			GetContext(gfx)->Unmap(dstres.get(), 0);

			DirectX::XMFLOAT3 norm_cur{float(x), float(y), mm.y};
			auto vec = DirectX::XMLoadFloat3(&norm_cur);
			vec = DirectX::XMVector3Unproject(vec,
				0, 0, w, h, 0, 1.0f, gfx.GetProjection(), gfx.GetCamera(), DirectX::XMMatrixIdentity());
			ctm.SetTransform(vec);

			//place in buffer
			pCursorBuffer->Update(gfx, { mm.x });
			pCursorBuffer->Bind(gfx);
			depthStencil->Clear(gfx);
		}
	private:
		std::shared_ptr<PixelConstantBuffer<B>> pCursorBuffer;
		StagingTexture region;
		mutable CursorTransform ctm;
	};
}