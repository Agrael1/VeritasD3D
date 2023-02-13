#pragma once
#include <Shared/Definitions.h>
#include <Game/Keyboard.h>
#include <Game/Mouse.h>
#include <wil/resource.h>

namespace ver
{
	class Window
	{	
	private:
		class WindowClass
		{
		public:
			static const char* GetName()noexcept
			{
				return wndClassName;
			}
			static HINSTANCE GetInstance()noexcept
			{
				return wndClass.hInst;
			}
		private:
			WindowClass() noexcept;
			~WindowClass()
			{
				UnregisterClass(wndClassName, GetInstance());
			}
			WindowClass(const WindowClass&) = delete;
			WindowClass& operator=(const WindowClass&) = delete;
			static inline constexpr const char* wndClassName = "Veritas Direct3D Window";
			static WindowClass wndClass;
			HINSTANCE hInst;
		};
	public:
		Window() = default;
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
	public:
		void Initialize(int32_t width, int32_t height, std::string_view name);
		uint32_t GetWidth() const noexcept
		{
			return width;
		}
		uint32_t GetHeight() const noexcept
		{
			return height;
		}

		void EnableCursor() noexcept
		{
			cursor_enabled = true;
			ShowCursor();
			FreeCursor();
		}
		void DisableCursor() noexcept
		{
			cursor_enabled = false;
			HideCursor();
			ConfineCursor();
		}
		bool CursorEnabled() const noexcept
		{
			return cursor_enabled;
		}
		bool ResizeCalled() const noexcept
		{
			return resized;
		}
		void ResizeComplete()noexcept
		{
			resized = false;
		}
		bool IsActive()const noexcept
		{
			return active;
		}
		void SetTitle(std::string_view title)
		{
			winrt::check_bool(SetWindowText(hWnd.get(), title.data()));
		}
		void ChangeToFullScreen();

		HWND GetHandle()const noexcept { return hWnd.get(); }

		std::optional<WPARAM> ProcessMessages()const noexcept
		{
			MSG msg;
			while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
			{
				if (msg.message == WM_QUIT)
					return msg.wParam;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if (!active)
				WaitMessage();

			return{};
		}
	private:
		static LRESULT WINAPI HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
		static LRESULT WINAPI HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
		{
			// retrieve ptr to win class
			Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			// forward msg to class handler
			return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
		}
		LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		void ConfineCursor() noexcept
		{
			RECT rect;
			GetClientRect(hWnd.get(), &rect);
			MapWindowPoints(hWnd.get(), nullptr, reinterpret_cast<POINT*>(&rect), 2);
			ClipCursor(&rect);
		}
		void FreeCursor() noexcept
		{
			ClipCursor(nullptr);
		}
		void ShowCursor() noexcept
		{
			while (::ShowCursor(true) < 0);
		}
		void HideCursor() noexcept
		{
			while (::ShowCursor(false) >= 0);
		}
	public:
		Keyboard kbd;
		Mouse mouse;
	private:
		bool cursor_enabled = true;
		bool resized = false;
		bool active = true;
		int32_t width = 640;
		int32_t height = 480;
		wil::unique_hwnd hWnd;
		std::vector<BYTE> rawBuffer;
	};
}