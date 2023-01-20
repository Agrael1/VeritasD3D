#pragma once
#include <Shared/Definitions.h>
#include <string>
#include <wtypes.h>
#include <wil/resource.h>

namespace ver
{
	class Window
	{
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
			static inline constexpr const char* wndClassName = "Veritas Direct3D Window";
			static WindowClass wndClass;
			HINSTANCE hInst;
		};
	public:
		Window() = default;
		Window(uint32_t width, uint32_t height, std::string_view name);
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;
	public:
		void Initialize(uint32_t width, uint32_t height, std::string_view name);
		uint32_t GetWidth() const noexcept;
		uint32_t GetHeight() const noexcept;

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
		void ChangeToFullScreen();

		bool RestyleCalled()const noexcept;
		void RestyleComplete()noexcept;
		//HWND GetHandle()const noexcept { return hWnd.get(); }

		std::optional<WPARAM> ProcessMessages()const noexcept;
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
		//Keyboard kbd;
		//Mouse mouse;
	private:
		//bool cursorEnabled = true;
		//bool bLoadCallIssued = false;
		//bool bGridEnabled = true;
		//bool bResizeIssued = false;
		//bool bRestyleIssued = false;
		bool active = true;
		uint32_t width = 640;
		uint32_t height = 480;
		//Style style;
		wil::unique_hwnd hWnd;
		//wil::unique_hmenu menu;
		//wil::unique_hmenu FileMenu;
		//wil::unique_hmenu OptionsMenu;
		//wil::unique_hmenu StylesMenu;
		//wil::unique_haccel Accelerator;
		//std::vector<BYTE> rawBuffer;
	};
}