#pragma once
#include <Shared/DXGIInfoManager.h>
#include <Shared/Timer.h>
#include <DirectXMath.h>
#include <memory>
#include <d3d11_4.h>

namespace ver
{
	class GraphicsResource;
	class Gizmo;
}

class OutputOnlyRenderTarget;

class Graphics
{
	inline static constexpr auto num_frames = 2;
	inline static winrt::com_ptr<IDXGIFactory4> factory;
	friend class ver::GraphicsResource;
	friend class ver::Gizmo;
public:
	enum Cam
	{
		left,
		right,
		central,
	};
public:
	Graphics(uint32_t width, uint32_t height, bool software = false);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
public:
	ver::IAsyncAction CreateSwapChain(HWND wnd);

	void EnableImgui()noexcept
	{
		imguiEnabled = true;
	}
	void DisableImgui()noexcept
	{
		imguiEnabled = false;
	}
	bool IsImguiEnabled()const noexcept
	{
		return imguiEnabled;
	}
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
	void SetCentralCamera(DirectX::XMMATRIX Camera)noexcept
	{
		ccamera = Camera;
	}
	void SetRightCamera(DirectX::XMMATRIX Camera)noexcept
	{
		rcamera = Camera;
	}
	void SetCamera(Cam c = Cam::left)noexcept
	{
		switch (c)
		{
		case Graphics::left:
			camera = lcamera;
			break;
		case Graphics::right:
			camera = rcamera;
			break;
		default:
			camera = ccamera;
			break;
		}
	}
	void SetCursor(std::pair<int, int> cur)noexcept
	{
		cursor = cur;
	}
	auto GetCursor()const noexcept
	{
		return cursor;
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
	std::shared_ptr<OutputOnlyRenderTarget> GetRightTarget() { return stereoEnabled?pRightTarget: pLeftTarget; }
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
	ver::DXGIInfoManager infoManager;

	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	DirectX::XMMATRIX lcamera;
	DirectX::XMMATRIX rcamera;
	DirectX::XMMATRIX ccamera;
	DirectX::XMFLOAT3 shadowPos;
	std::pair<int, int> cursor{0,0};

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