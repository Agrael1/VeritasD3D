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
	friend class ver::GraphicsResource;
public:
	Graphics(HWND hWnd, uint32_t width, uint32_t height);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
public:
	void EnableImgui()noexcept;
	void DisableImgui()noexcept;
	bool IsImguiEnabled()const noexcept;
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

	std::shared_ptr<OutputOnlyRenderTarget> GetTarget(){return pTarget;}
	void OnResize(unsigned newwidth, unsigned newheight);
	void DrawIndexed(uint32_t count)noxnd;
	void Draw(uint32_t vcount)noxnd;
private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
	DirectX::XMFLOAT3 shadowPos;
#ifndef NDEBUG
	ver::DXGIInfoManager infoManager;
#endif
	bool imguiEnabled = true;
private:
	ver::timer timer;
	winrt::com_ptr<ID3D11Device> pDevice;
	winrt::com_ptr<IDXGISwapChain> pSwap;
	winrt::com_ptr<ID3D11DeviceContext> pContext;
	std::shared_ptr<OutputOnlyRenderTarget> pTarget;
	uint32_t width;
	uint32_t height;
	float frame_step = 0.0f;
};