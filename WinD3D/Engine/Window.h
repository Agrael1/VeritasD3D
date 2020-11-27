#pragma once
#include <Framework\Exception.h>

#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <memory>
#include <optional>

class Window 
{
public:
	class WindowException :public Exception
	{
		using Exception::Exception;
	public:
		static std::string TranslateErrorCode(HRESULT hr)noexcept;
	};
	class HrException :public WindowException
	{
	public:
		HrException(int line, const char* file, HRESULT hr);
		const char* what() const noexcept override;
		const char* GetType()const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::string GetErrorDescription() const noexcept;
	private:
		HRESULT hResult;
	};
	class NoGfxException :public WindowException
	{
	public:
		using WindowException::WindowException;
		const char* GetType()const noexcept override;
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
		WindowClass(const WindowClass&) = delete;
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
public:
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	bool LoadCalled() const noexcept;
	void LoadingComplete()noexcept;
	void SetTitle(std::string_view title);
	static std::optional<WPARAM> ProcessMessages()noexcept;
	Graphics& Gfx();
private:
	static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void ConfineCursor() noexcept;
	void FreeCursor() noexcept;
	void ShowCursor() noexcept;
	void HideCursor() noexcept;
	void EnableImGuiMouse() noexcept;
	void DisableImGuiMouse() noexcept;
	void AddMenu(HWND hWnd);
public:
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = true;
	bool bLoadCallIssued = false;
	int width;
	int height;
	HWND hWnd;
	HMENU menu;
	std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
};

#define WND_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define WND_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define WND_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )
#define WND_CALL_INFO(call) if(!(call)) throw(WND_LAST_EXCEPT())