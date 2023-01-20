#include <Game/Window.h>
#include <Shared/Checks.h>
#include <resource.h>

ver::Window::WindowClass ver::Window::WindowClass::wndClass;

ver::Window::WindowClass::WindowClass() noexcept
	:hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wcWindow = { 0 };
	wcWindow.cbSize = sizeof(wcWindow);
	wcWindow.style = CS_OWNDC;
	wcWindow.lpfnWndProc = HandleMsgSetup;
	wcWindow.cbClsExtra = 0;
	wcWindow.cbWndExtra = 0;
	wcWindow.hInstance = GetInstance();
	wcWindow.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcWindow.hIcon = LoadIcon(wcWindow.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcWindow.hbrBackground = nullptr;
	wcWindow.lpszMenuName = nullptr;
	wcWindow.hIconSm = nullptr;
	wcWindow.lpszClassName = GetName();
	RegisterClassExA(&wcWindow);
}
ver::Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}
const char* ver::Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}
HINSTANCE ver::Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

void ver::Window::Initialize(uint32_t width, uint32_t height, std::string_view name)
{
	RECT rWindow;
	rWindow.left = 100;
	rWindow.right = width + rWindow.left;
	rWindow.top = 100;
	rWindow.bottom = height + rWindow.top;
	// Automatic calculation of window height and width to client region
	winrt::check_bool(AdjustWindowRect(&rWindow, WS_OVERLAPPEDWINDOW, TRUE));
	hWnd.reset(CreateWindowA(
		WindowClass::GetName(), name.data(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rWindow.right - rWindow.left,
		rWindow.bottom - rWindow.top,
		nullptr, nullptr,
		WindowClass::GetInstance(), this
	));

	winrt::check_bool(bool(hWnd));
	ShowWindow(hWnd.get(), SW_SHOWDEFAULT);

	// Init GUI (only one window supported)
	//winrt::check_bool(ImGui_ImplWin32_Init(hWnd.get()));

	RAWINPUTDEVICE rid{};
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	winrt::check_bool(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
}


std::optional<WPARAM> ver::Window::ProcessMessages()const noexcept
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
LRESULT WINAPI ver::Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// Create routine initializer
	if (msg == WM_NCCREATE)
	{
		// Extract data from creation of window
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
		Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
		// set WinAPI-managed user data to store ptr to win class
		SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
		// set msgproc to to non setup handle
		SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));

		return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WINAPI ver::Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to win class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward msg to class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}
LRESULT ver::Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, msg, wParam, lParam);
}