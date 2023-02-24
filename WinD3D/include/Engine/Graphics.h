#pragma once
#include <Engine/Util/DXGIInfoManager.h>
#include <DirectXMath.h>
#include <memory>

namespace ver
{
	class GraphicsResource;
}

class OutputOnlyRenderTarget;

class Graphics
{
	inline static constexpr auto num_frames = 2;
	inline static winrt::com_ptr<IDXGIFactory4> factory;
	friend class ver::GraphicsResource;
public:
	Graphics(uint32_t width, uint32_t height, bool software = false);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
public:
	winrt::IAsyncAction CreateSwapChain(HWND wnd);

	void EnableImgui()noexcept;
	void DisableImgui()noexcept;
	bool IsImguiEnabled()const noexcept;
	bool StereoEnabled()const noexcept
	{
		return stereoEnabled;
	}
	void BeginFrame(float r, float g, float b)noexcept;
	void EndFrame();

	DirectX::XMMATRIX GetCamera()const noexcept
	{
		return camera;
	}
	void SetCamera(DirectX::XMMATRIX Camera)noexcept
	{
		camera = Camera;
	}
	void SetLeftCamera(DirectX::XMMATRIX Camera)noexcept
	{
		lcamera = Camera;
	}
	void SetRightCamera(DirectX::XMMATRIX Camera)noexcept
	{
		rcamera = Camera;
	}
	void SetCamera(bool left)noexcept
	{
		camera = left ? lcamera : rcamera;
		//camera = lcamera;
	}

	DirectX::XMMATRIX GetProjection() const noexcept
	{
		return projection;
	}
	void SetProjection(DirectX::FXMMATRIX proj) noexcept
	{
		projection = proj;
	}
	float GetFrameStep()const noexcept
	{
		return frame_step;
	}
	DirectX::XMVECTOR GetShadowCamPos()const noexcept
	{
		return DirectX::XMLoadFloat3(&shadowPos);
	}
	void SetShadowCamPos(DirectX::FXMVECTOR v)noexcept
	{
		return DirectX::XMStoreFloat3(&shadowPos, v);
	}

	uint32_t GetWidth() const noexcept { return width; }
	uint32_t GetHeight() const noexcept { return height; }

	std::shared_ptr<OutputOnlyRenderTarget> GetTarget() { return pLeftTarget; } 
	std::shared_ptr<OutputOnlyRenderTarget> GetLeftTarget() { return GetTarget(); }
	std::shared_ptr<OutputOnlyRenderTarget> GetRightTarget() { return pRightTarget; }
	auto RawDevice()const noexcept { return pDevice; }
	auto RawContext()const noexcept { return pContext; }

	void OnResize(unsigned newwidth, unsigned newheight);
	void DrawIndexed(uint32_t count)noxnd;
	void Draw(uint32_t vcount)noxnd;
private:
	bool StereoStatus();
	void GetHardwareAdapter();
	void GetSoftwareAdapter();
private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX lcamera;
	DirectX::XMMATRIX rcamera;
	DirectX::XMFLOAT3 shadowPos;
#ifndef NDEBUG
	ver::DXGIInfoManager infoManager;
#endif
	bool imguiEnabled = true;
	bool stereoEnabled = false;
private:
	ver::timer timer;
	winrt::com_ptr<ID3D11Device> pDevice;
	winrt::com_ptr<IDXGISwapChain3> pSwap;
	winrt::com_ptr<ID3D11DeviceContext> pContext;
	std::shared_ptr<OutputOnlyRenderTarget> pLeftTarget;
	std::shared_ptr<OutputOnlyRenderTarget> pRightTarget;
	uint32_t width;
	uint32_t height;
	float frame_step = 0.0f;
};