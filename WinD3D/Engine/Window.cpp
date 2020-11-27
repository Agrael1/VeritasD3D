#include "Window.h"
#include <sstream>
#include "resource.h"
#include "ImGUI\imgui_impl_win32.h"


Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:hInst(GetModuleHandle(nullptr))
{
	WNDCLASSEX wcWindow = { 0 };
	wcWindow.cbSize = sizeof(wcWindow);
	wcWindow.style = CS_OWNDC;
	wcWindow.lpfnWndProc = HandleMsgSetup;
	wcWindow.cbClsExtra = 0;
	wcWindow.cbWndExtra = 0;
	wcWindow.hInstance = GetInstance();
	wcWindow.hCursor = nullptr;
	wcWindow.hIcon = LoadIcon(wcWindow.hInstance,MAKEINTRESOURCE(IDI_ICON2));
	wcWindow.hbrBackground = nullptr;
	wcWindow.lpszMenuName = nullptr;
	wcWindow.hIconSm = nullptr;
	wcWindow.lpszClassName = GetName();
	RegisterClassExA(&wcWindow);
}
Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}
const char * Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}
HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

// Window namespace
Window::Window(unsigned int width, unsigned int height, const char * name):width(width),height(height)
{
	RECT rWindow;
	rWindow.left = 100;
	rWindow.right = width + rWindow.left;
	rWindow.top = 100;
	rWindow.bottom = height + rWindow.top;
	// Automatic calculation of window height and width to client region
	WND_CALL_INFO(AdjustWindowRect(&rWindow, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, TRUE));

	hWnd = CreateWindowA(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		rWindow.right - rWindow.left,
		rWindow.bottom - rWindow.top,
		nullptr, nullptr,
		WindowClass::GetInstance(), this
	);

	// Error checks
	if (!hWnd) throw WND_LAST_EXCEPT();
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	
	// Init GUI (only one window supported)
	WND_CALL_INFO(ImGui_ImplWin32_Init(hWnd));

	// Create Graphics object
	pGfx = std::make_unique<Graphics>(hWnd, width, height);

	RAWINPUTDEVICE rid;
	rid.usUsagePage = 0x01; // mouse page
	rid.usUsage = 0x02; // mouse usage
	rid.dwFlags = 0;
	rid.hwndTarget = nullptr;
	WND_CALL_INFO(RegisterRawInputDevices(&rid, 1, sizeof(rid)));
}
Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}
void Window::SetTitle(std::string_view title)
{
	if (!SetWindowText(this->hWnd, title.data()))
	{
		throw WND_LAST_EXCEPT();
	}
}

void Window::EnableCursor() noexcept
{
	cursorEnabled = true;
	ShowCursor();
	EnableImGuiMouse();
	FreeCursor();
}
void Window::DisableCursor() noexcept
{
	cursorEnabled = false;
	HideCursor();
	DisableImGuiMouse();
	ConfineCursor();
}
bool Window::CursorEnabled() const noexcept
{
	return cursorEnabled;
}

bool Window::LoadCalled() const noexcept
{
	return bLoadCallIssued;
}

void Window::LoadingComplete() noexcept
{
	bLoadCallIssued = false;
}

void Window::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	MapWindowPoints(hWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
	ClipCursor(&rect);
}
void Window::FreeCursor() noexcept
{
	ClipCursor(nullptr);
}
void Window::HideCursor() noexcept
{
	while (::ShowCursor(FALSE) >= 0);
}
void Window::ShowCursor() noexcept
{
	while (::ShowCursor(TRUE) < 0);
}

void Window::EnableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}
void Window::DisableImGuiMouse() noexcept
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
}

void Window::AddMenu(HWND hWnd)
{
	menu = CreateMenu();
	HMENU hFileMenu = CreateMenu();
	AppendMenu(hFileMenu, MF_STRING, 0, TEXT("Load"));
	AppendMenu(menu, MF_POPUP, (UINT_PTR)hFileMenu, TEXT("File"));
	SetMenu(hWnd, menu);
}

std::optional<WPARAM> Window::ProcessMessages() noexcept
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return msg.wParam;
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return{};
}
Graphics & Window::Gfx()
{
	if (!pGfx)
	{
		throw WND_NOGFX_EXCEPT();
	}
	return *pGfx;
}
LRESULT WINAPI Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	return DefWindowProc(hWnd,msg,wParam,lParam);
}
LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to win class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward msg to class handler
	return pWnd->HandleMsg(hWnd,msg,wParam,lParam);
}
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
	{
		return true;
	}
	const auto& imio = ImGui::GetIO();

	switch (msg)
	{
		// we don't want the DefProc to handle this message because
		// we want our destructor to destroy the window, so return 0 instead of break
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		// clear keystate when window loses focus to prevent input getting "stuck"
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	case WM_CREATE:
		AddMenu(hWnd);
		break;
	case WM_COMMAND:
		if (wParam == 0)
		{
			bLoadCallIssued = true;
		}
		break;
	case WM_ACTIVATE:
		// confine/free cursor on window to foreground/background if cursor disabled
		if (!cursorEnabled)
		{
			if (wParam & WA_ACTIVE)
			{
				ConfineCursor();
				HideCursor();
			}
			else
			{
				FreeCursor();
				ShowCursor();
			}
		}
		break;

		/*********** KEYBOARD MESSAGES ***********/
	case WM_KEYDOWN:
		// syskey commands need to be handled to track ALT key (VK_MENU) and F10
	case WM_SYSKEYDOWN:
		// stifle this keyboard message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
		{
			break;
		}
		if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) // filter autorepeat
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		// stifle this keyboard message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		// stifle this keyboard message if imgui wants to capture
		if (imio.WantCaptureKeyboard)
		{
			break;
		}
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
		/*********** END KEYBOARD MESSAGES ***********/

		/************* MOUSE MESSAGES ****************/
	case WM_MOUSEMOVE:
	{
		const POINTS pt = MAKEPOINTS(lParam);
		// cursorless exclusive gets first dibs
		if (!cursorEnabled)
		{
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
				HideCursor();
			}
			break;
		}
		// stifle this mouse message if imgui wants to capture
		if (imio.WantCaptureMouse)
		{
			break;
		}
		// in client region -> log move, and log enter + capture mouse (if not previously in window)
		if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height)
		{
			mouse.OnMouseMove(pt.x, pt.y);
			if (!mouse.IsInWindow())
			{
				SetCapture(hWnd);
				mouse.OnMouseEnter();
			}
		}
		// not in client -> log move / maintain capture if button down
		else
		{
			if (wParam & (MK_LBUTTON | MK_RBUTTON))
			{
				mouse.OnMouseMove(pt.x, pt.y);
			}
			// button up -> release capture / log event for leaving
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}
		break;
	}
	case WM_LBUTTONDOWN:
	{
		SetForegroundWindow(hWnd);
		if (!cursorEnabled)
		{
			ConfineCursor();
			HideCursor();
		}
		// stifle this mouse message if imgui wants to capture
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftPressed(pt.x, pt.y);
		break;
	}
	case WM_RBUTTONDOWN:
	{
		// stifle this mouse message if imgui wants to capture
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightPressed(pt.x, pt.y);
		break;
	}
	case WM_LBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnLeftReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_RBUTTONUP:
	{
		// stifle this mouse message if imgui wants to capture
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		mouse.OnRightReleased(pt.x, pt.y);
		// release mouse if outside of window
		if (pt.x < 0 || pt.x >= width || pt.y < 0 || pt.y >= height)
		{
			ReleaseCapture();
			mouse.OnMouseLeave();
		}
		break;
	}
	case WM_MOUSEWHEEL:
	{
		// stifle this mouse message if imgui wants to capture
		if (imio.WantCaptureMouse)
		{
			break;
		}
		const POINTS pt = MAKEPOINTS(lParam);
		const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
		mouse.OnWheelDelta(pt.x, pt.y, delta);
		break;
	}
	/************** END MOUSE MESSAGES **************/

	/************** RAW MOUSE MESSAGES **************/
	case WM_INPUT:
	{
		if (!mouse.RawEnabled())
		{
			break;
		}
		UINT size = 0;
		// first get the size of the input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			nullptr,
			&size,
			sizeof(RAWINPUTHEADER)) == -1)
		{
			// bail msg processing if error
			break;
		}
		rawBuffer.resize(size);
		// read in the input data
		if (GetRawInputData(
			reinterpret_cast<HRAWINPUT>(lParam),
			RID_INPUT,
			rawBuffer.data(),
			&size,
			sizeof(RAWINPUTHEADER)) != size)
		{
			// bail msg processing if error
			break;
		}
		// process the raw input data
		auto& ri = reinterpret_cast<const RAWINPUT&>(*rawBuffer.data());
		if (ri.header.dwType == RIM_TYPEMOUSE &&
			(ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
		{
			mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);
		}
		break;
	}
	/************** END RAW MOUSE MESSAGES **************/
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

//Window Exception
Window::HrException::HrException(int line, const char * file, HRESULT hr)
	:WindowException(line,file),hResult(hr)
{}
std::string Window::WindowException::TranslateErrorCode(HRESULT hr) noexcept
{
	char* pMsgBuf = nullptr;
	DWORD nMsgLen = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&pMsgBuf),
		0, nullptr);

	if (nMsgLen == 0)
	{
		return "Unknown error";
	}
	std::string errorString = pMsgBuf;
	LocalFree(pMsgBuf);
	return errorString;
}
HRESULT Window::HrException::GetErrorCode() const noexcept
{
	return hResult;
}
std::string Window::HrException::GetErrorDescription() const noexcept
{
	return WindowException::TranslateErrorCode(hResult);
}
const char* Window::HrException::GetType()const noexcept
{
	return "Vertas Window Exception";
}
const char* Window::HrException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode()
		<< std::dec << " (" << (unsigned long)GetErrorCode() << ")" << std::endl
		<< "[Description] " << GetErrorDescription() << std::endl
		<< GetOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}
const char* Window::NoGfxException::GetType() const noexcept
{
	return "Veritas Window Exception [No Graphics]";
}