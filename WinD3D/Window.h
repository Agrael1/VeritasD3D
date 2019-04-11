#pragma once

#include "Engine\Window.h"
#include "Engine\Exception.h"

class Window 
{
public:
	class WindowException :public Exception
	{
	public:
		WindowException(int line, const char* file,HRESULT hr);
		const char* what() const noexcept override;
		virtual const char* GetType()const noexcept override;
		static std::string TranslateErrorCode(HRESULT hr)noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorString() const noexcept;
	private:
		HRESULT hResult;
	};
private:
	// Registration of window
	class WindowClass
	{
	public:
		static const char* GetName()noexcept;
		static HINSTANCE GetInstance()noexcept;
	private:
		WindowClass() noexcept;
		~WindowClass();
		WindowClass(const WindowClass&) = delete;				// non copyable class
		WindowClass& operator=(const WindowClass&) = delete;
		static constexpr const char* wndClassName = "Veritas Direct3D Window";
		static Window::WindowClass wndClass;
		HINSTANCE hInst;
	};
public:
	Window(unsigned int width, unsigned int height, const char* name);
	~Window();
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
private:
	static LRESULT __stdcall HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT __stdcall HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
private:
	int width;
	int height;
	HWND hWnd;
};

#define WND_EXCEPT(hr) Window::WindowException(__LINE__,__FILE__,hr)
#define WND_EXCEPT_AUTO() Window::WindowException(__LINE__,__FILE__,GetLastError())