#pragma once
#include <Engine/Pass/RenderQueuePass.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Bindable/ShadowRasterizer.h>
#include <Engine/Bindable/ShadowSampler.h>
#include <Engine/Bindable/NullPixelShader.h>
#include <Engine/Bindable/CubeTexture.h>
#include <Engine/Bindable/Viewport.h>


class Graphics;

using namespace RG;

namespace ver::rg
{
	class ShadowMappingPass : public RenderQueuePass
	{
	public:
		ShadowMappingPass(Graphics& gfx, std::string name)
			:
			RenderQueuePass(std::move(name))
		{
			pDepthCube = std::make_shared<DepthCubeTexture>(gfx, size, 5);
			AddBind(VertexShader::Resolve(gfx, "shadow.vs.cso"));
			AddBind(NullPixelShader::Resolve(gfx));
			AddBind(Stencil::Resolve(gfx, Stencil::Mode::Off));
			AddBind(BlendState::Resolve(gfx, false));
			AddBind(std::make_shared<Viewport>(gfx, (float)size, (float)size));
			AddBind(std::make_shared<ShadowRasterizer>(gfx, 50, 2.0f, 0.1f));
			RegisterSource(DirectBindableSource<DepthCubeTexture>::Make("map", pDepthCube));

			DirectX::XMStoreFloat4x4(
				&projection,
				DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, 0.5f, 1000.0f)
			);
			// +x
			DirectX::XMStoreFloat3(&cameraDirections[0], DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[0], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// -x
			DirectX::XMStoreFloat3(&cameraDirections[1], DirectX::XMVectorSet(-1.0f, 0.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[1], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// +y
			DirectX::XMStoreFloat3(&cameraDirections[2], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[2], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			// -y
			DirectX::XMStoreFloat3(&cameraDirections[3], DirectX::XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[3], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			// +z
			DirectX::XMStoreFloat3(&cameraDirections[4], DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[4], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
			// -z
			DirectX::XMStoreFloat3(&cameraDirections[5], DirectX::XMVectorSet(0.0f, 0.0f, -1.0f, 0.0f));
			DirectX::XMStoreFloat3(&cameraUps[5], DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

			SetDepthBuffer(pDepthCube->GetDepthBuffer(0));
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			using namespace DirectX;

			const auto pos = gfx.GetShadowCamPos();
			auto old_proj = gfx.GetProjection();
			auto old_cam = gfx.GetCamera();

			gfx.SetProjection(XMLoadFloat4x4(&projection));
			for (size_t i = 0; i < 6; i++)
			{
				auto d = pDepthCube->GetDepthBuffer(i);
				d->Clear(gfx);
				SetDepthBuffer(std::move(d));
				const auto lookAt = pos + XMLoadFloat3(&cameraDirections[i]);
				gfx.SetCamera(XMMatrixLookAtLH(pos, lookAt, XMLoadFloat3(&cameraUps[i])));
				RenderQueuePass::Execute(gfx);
			}

			gfx.SetCamera(old_cam);
			gfx.SetProjection(old_proj);
		}
	private:
		void SetDepthBuffer(std::shared_ptr<DepthStencil> ds) const
		{
			const_cast<ShadowMappingPass*>(this)->depthStencil = std::move(ds);
		}
		static inline constexpr UINT size = 1024;
		std::shared_ptr<DepthCubeTexture> pDepthCube;
		DirectX::XMFLOAT4X4 projection;
		std::array<DirectX::XMFLOAT3, 6> cameraDirections;
		std::array<DirectX::XMFLOAT3, 6> cameraUps;
	};
}