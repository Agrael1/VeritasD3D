#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include <Game/Menu.h>
#include <Engine/Graphics.h>
#include <optional>

namespace ver
{
	class FileOpenDialog;
}

enum class Event : uint8_t
{
	Resize,
	Restyle,
	LoadAsset,
	Play,
	Count
};
constexpr inline auto operator+(Event e) { return std::to_underlying(e); }

struct EventSet
{
	void push(Event e)
	{
		if (map[+e])return;
		horizon[iter++] = e;
		map.set(+e);
	}
	auto begin() { return horizon.begin(); }
	auto end() { return horizon.begin() + iter; }
	void clear() { iter = 0; map.reset(); }
	std::array<Event, +Event::Count> horizon{};
	std::bitset<+Event::Count> map{}; 
	uint8_t iter = 0;
};

class Window
{
	friend class ver::FileOpenDialog;
public:
	struct EventQueue
	{
	public:
		EventQueue(EventSet& events):events(events){}
		~EventQueue(){events.clear();}
	public:
		auto begin() { return events.begin(); }
		auto end() { return events.end(); }
	private:
		EventSet& events;
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
	[[nodiscard]]
	EventQueue GetEvents()noexcept { return { events }; }
	void EnableLoading();

	int GetWidth() const noexcept { return width; }
	int GetHeight() const noexcept { return height; }
	void EnableCursor() noexcept;
	void DisableCursor() noexcept;
	bool CursorEnabled() const noexcept;
	bool DrawGrid()const noexcept;
	bool IsActive()const noexcept;
	void SetTitle(std::string_view title);
	void ChangeToFullScreen();

	auto GetStyle()const noexcept { return menu.GetStyle(); }
	HWND GetHandle()const noexcept { return hWnd.get(); }

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
	Keyboard kbd;
	Mouse mouse;
private:
	bool cursorEnabled = true;
	bool bGridEnabled = true;
	bool bActive = true;
	int width;
	int height;
	wil::unique_hwnd hWnd;
	UT::Menu menu;
	wil::unique_haccel Accelerator;

	std::vector<BYTE> rawBuffer;
	EventSet events;
};

