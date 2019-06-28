#pragma once
#include "Engine\WinSetup.h"
#include "Engine\Exception.h"
#include <d3d11.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
//#ifndef NDEBUG
#include "DXGIInfoManager.h"
//#endif

class Graphics
{
	friend class Bindable;
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
	class ContextException : public GException
	{
	public:
		ContextException(int line, const char* file, std::vector<std::string> messages = {})noexcept;
		const char* what()const noexcept override;
		const char* GetType()const noexcept override;
		std::string GetErrorInfo()const noexcept;
	private:
		std::string info;
	};
public:
	Graphics( HWND hWnd);
	Graphics(const Graphics&) = delete;
	Graphics& operator=(const Graphics&) = delete;
	~Graphics();
public:
	void EnableImgui()noexcept;
	void DisableImgui()noexcept;
	bool IsImguiEnabled()const noexcept;
	void BeginFrame(float r, float g, float b)noexcept;
	void EndFrame();
	//void ClearBuffer(float r, float g, float b)noexcept;
	void DrawTestFigure(float angle, float x, float y);
	void DrawIndexed(UINT count)noexcept(!IS_DEBUG);
	DirectX::XMMATRIX GetProjection() const noexcept;
	void SetProjection(DirectX::FXMMATRIX proj) noexcept;
private:
	DirectX::XMMATRIX projection;
#ifndef NDEBUG
	DXGIInfoManager infoManager;
#endif
	bool imguiEnabled;
private:
	Microsoft::WRL::ComPtr<ID3D11Device> pDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain> pSwap;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> pContext;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> pTarget;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> pDSV;
};