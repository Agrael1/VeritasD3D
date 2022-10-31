#pragma once
#include <Engine/Util/WindowExceptions.h>

#include "Keyboard.h"
#include "Mouse.h"
#include <Engine/Graphics.h>
#include <memory>
#include <optional>

namespace ver
{
	class FileOpenDialog;
}

class Window
{
	friend class ver::FileOpenDialog;
public:
	enum class Style
	{
		VGUI,
		Dark,
		Cherry
	};
private:
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
	UINT GetWidth() const noexcept;
	UINT GetHeight() const noexcept;
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	bool LoadCalled() const noexcept;
	void LoadingComplete()noexcept;
	bool ResizeCalled() const noexcept;
	void ResizeComplete()noexcept;
	bool DrawGrid()const noexcept;
	bool IsActive()const noexcept;
	void SetTitle(std::string_view title);

	bool RestyleCalled()const noexcept;
	void RestyleComplete()noexcept;
	Style GetStyle()const noexcept;

	std::optional<WPARAM> ProcessMessages()const noexcept;
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
public:
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = true;
	bool bLoadCallIssued = false;
	bool bGridEnabled = true;
	bool bResizeIssued = false;
	bool bRestyleIssued = false;
	bool bActive = true;
	int width;
	int height;
	Style style;
	wil::unique_hwnd hWnd;
	wil::unique_hmenu menu;
	wil::unique_hmenu FileMenu;
	wil::unique_hmenu OptionsMenu;
	wil::unique_hmenu StylesMenu;
	wil::unique_haccel Accelerator;
	std::unique_ptr<Graphics> pGfx;
	std::vector<BYTE> rawBuffer;
};

