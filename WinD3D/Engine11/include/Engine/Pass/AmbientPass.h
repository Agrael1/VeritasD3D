#pragma once
#include <GFSDK_SSAO.h>
#include <Engine/Pass/BindingPass.h>
#include <Engine/Bindable/BindableCommons.h>
#include <Engine/Rendering/SinksT.h>
#include <Engine/Rendering/SourcesT.h>
#include <Engine/Bindable/DepthStencil.h>
#include <Engine/Bindable/RenderTarget.h>
#include <Engine/Graphics.h>

namespace ver::rg
{
	class AmbientPass : public RG::Pass, public ::ver::GraphicsResource
	{
	public:
		AmbientPass(Graphics& gfx, std::string name)
			:Pass(std::move(name))
		{
			RegisterSink(RG::DirectBindableSink<IRenderTarget>::Make("targets", gbuf));
			RegisterSink(RG::DirectBufferSink<ver::DepthStencil>::Make("depthStencil", depthStencil));
			RegisterSource(RG::DirectBindableSource<IRenderTarget>::Make("targets", gbuf));
			RegisterSource(RG::DirectBufferSource<ver::DepthStencil>::Make("depthStencil", depthStencil));

			GFSDK_SSAO_Status status;	
			status = GFSDK_SSAO_CreateContext_D3D11(GetDevice(gfx), pAOContext.put());
			assert(status == GFSDK_SSAO_OK); // HBAO+ requires feature level 11_0 or above
			
			auto r = DirectX::XMMatrixIdentity();

			Input.DepthData.DepthTextureType = GFSDK_SSAO_HARDWARE_DEPTHS;
			Input.DepthData.ProjectionMatrix.Layout = GFSDK_SSAO_ROW_MAJOR_ORDER;
			Input.DepthData.MetersToViewSpaceUnits = 1.0f;
			Input.NormalData.WorldToViewMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)&r);
			Input.NormalData.Enable = true;
			Input.NormalData.DecodeScale = 0.2f;
			
			Params.Radius = 2.f;
			Params.Bias = 0.3f;
			Params.PowerExponent = 2.f;
			Params.Blur.Enable = true;
			Params.Blur.Radius = GFSDK_SSAO_BLUR_RADIUS_4;
			Params.Blur.Sharpness = 16.f;

			Output.Blend.Mode = GFSDK_SSAO_MULTIPLY_RGB;
		}
		void Execute(Graphics& gfx) const noxnd override
		{
			GFSDK_SSAO_Status status;

			auto r = gfx.GetProjection();
			Input.DepthData.ProjectionMatrix.Data = GFSDK_SSAO_Float4x4((const GFSDK_SSAO_FLOAT*)&r);

			Input.NormalData.pFullResNormalTextureSRV = std::static_pointer_cast<::RenderTargetArray>(gbuf)->GetSRV(1);
			Output.pRenderTargetView = std::static_pointer_cast<::RenderTargetArray>(gbuf)->GetRTV(0);
			Input.DepthData.pFullResDepthTextureSRV = std::static_pointer_cast<ver::ShaderInputDepthStencil>(depthStencil)->GetSRV();
			status = pAOContext->RenderAO(GetContext(gfx), Input, Params, Output);
			assert(status == GFSDK_SSAO_OK);
		}
	private:
		std::shared_ptr<IRenderTarget> gbuf;
		std::shared_ptr<DepthStencil> depthStencil;
		winrt::com_ptr<GFSDK_SSAO_Context_D3D11> pAOContext;
		GFSDK_SSAO_Parameters Params;
		mutable GFSDK_SSAO_Output_D3D11 Output;
		mutable GFSDK_SSAO_InputData_D3D11 Input;
	};
}