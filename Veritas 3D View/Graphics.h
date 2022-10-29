#pragma once
#include "DXGIInfoManager.h"

class RenderTarget;

class Graphics
{
	friend class GraphicsResource;
public:
	Graphics(unsigned width, unsigned height, IUnknown* DrawSwapChain);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
public:
	void EnableImgui()noexcept;
	void DisableImgui()noexcept;
	bool IsImguiEnabled()const noexcept;
	void BeginFrame(float r, float g, float b)noexcept;
	void EndFrame();

	DirectX::XMMATRIX GetCamera()const noexcept;
	void SetCamera(DirectX::XMMATRIX Camera)noexcept;
	void DrawIndexed(UINT count)noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	std::shared_ptr<RenderTarget> GetTarget();
	void OnResize(unsigned newwidth, unsigned newheight);
private:
	DirectX::XMMATRIX projection;
	DirectX::XMMATRIX camera;
#ifndef NDEBUG
	DXGIInfoManager infoManager;
#endif
	bool imguiEnabled = true;
private:
	winrt::com_ptr<ID3D11Device> pDevice;
	winrt::com_ptr<IDXGISwapChain1> pSwap;
	winrt::com_ptr<ID3D11DeviceContext> pContext;
	std::shared_ptr<RenderTarget> pTarget;
	UINT width;
	UINT height;
};

