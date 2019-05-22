#pragma once
#include "Engine\WinSetup.h"
#include "Engine\Exception.h"
#include <d3d11.h>
#include <wrl.h>
#ifndef NDEBUG
#include "DXGIInfoManager.h"
#endif

#include "GraphicsThrows.m"

class Graphics
{
public: 
	class GException :public Exception
	{
		using Exception::Exception;
	};
	class HrException :public GException
	{
	public:
		HrException(int line, const char* file, HRESULT hr, std::vector<std::string> messages = {})noexcept;
		const char* what()const noexcept override;
		const char* GetType()const noexcept override;
		HRESULT GetErrorCode()const noexcept;
		std::string GetErrorString()const noexcept;
		std::string GetErrorDescription()const noexcept;
		std::string GetErrorInfo()const noexcept;
	private:
		HRESULT hr;
		std::string info;
	};
	class DeviceRemovedException : public HrException
	{
		using HrException::HrException;
	public:
		const char* GetType()const noexcept override;
	private:
		std::string reason;
	};
public:
	Graphics( HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics() = default;
	void EndFrame();
	void ClearBuffer(float r, float g, float b)noexcept;
private:
#ifndef NDEBUG
	DXGIInfoManager infoManager;
#endif
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
};