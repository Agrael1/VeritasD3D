#pragma once
#include <Engine/Util/DXGIInfoManager.h>
#include <DirectXMath.h>
#include <memory>

namespace ver
{
	class GraphicsResource;
}

class OutputOnlyRenderTarget;
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

	uint32_t GetWidth() const noexcept { return width; }
	uint32_t GetHeight() const noexcept { return height; }

	std::shared_ptr<OutputOnlyRenderTarget> GetTarget(){return pTarget;}
	void OnResize(unsigned newwidth, unsigned newheight);
	void DrawIndexed(uint32_t count)noxnd;
private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
#ifndef NDEBUG
	DXGIInfoManager infoManager;
#endif
	bool imguiEnabled = true;
private:
	winrt::com_ptr<ID3D11Device> pDevice;
	winrt::com_ptr<IDXGISwapChain> pSwap;
	winrt::com_ptr<ID3D11DeviceContext> pContext;
	std::shared_ptr<OutputOnlyRenderTarget> pTarget;
	uint32_t width;
	uint32_t height;
};