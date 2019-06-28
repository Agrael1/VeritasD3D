#include "Engine\Window.h"
#include <sstream>
#include "resource.h"
#include "ImGUI\imgui_impl_win32.h"

#define GET_X_LPARAM(lp)                        ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp)                        ((int)(short)HIWORD(lp))

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
	wcWindow.hCursor = NULL;
	wcWindow.hIcon = LoadIcon(wcWindow.hInstance,MAKEINTRESOURCE(IDI_ICON2));
	wcWindow.hbrBackground = NULL;
	wcWindow.lpszMenuName = NULL;
	wcWindow.hIconSm = NULL;
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
	if (!AdjustWindowRect(&rWindow, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))
	{
		throw WND_LAST_EXCEPT();
	}

	hWnd = CreateWindowA(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, 
		rWindow.right - rWindow.left,
		rWindow.bottom - rWindow.top,
		NULL, NULL,
		WindowClass::GetInstance(), this
	);

	// Error checks
	if (hWnd == nullptr)
		throw WND_LAST_EXCEPT();


	mouse.InitializeMouse(hWnd);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	
	// Init GUI (only one window supported)
	ImGui_ImplWin32_Init(hWnd);

	// Create Graphics object
	pGfx = std::make_unique<Graphics>(hWnd);
}
Window::~Window()
{
	ImGui_ImplWin32_Shutdown();
	DestroyWindow(hWnd);
}
void Window::SetTitle(const std::string & title)
{
	if (!SetWindowText(this->hWnd, title.c_str()))
	{
		throw WND_LAST_EXCEPT();
	}
}

std::pair<bool,WPARAM> Window::ProcessMessages()noexcept
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (msg.message == WM_QUIT)
		{
			return{ true,msg.wParam };
		}

		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return{ false,0 };
}
Graphics & Window::Gfx()
{
	if (!pGfx)
	{
		throw WND_NOGFX_EXCEPT();
	}
	return *pGfx;
}
LRESULT __stdcall Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
LRESULT __stdcall Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
	// if the focus is lost, clear all zombie keys
	case WM_KILLFOCUS:
		kbd.ClearState();
		break;
	//Keyboard messages
	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if (!(lParam & 0x40000000))
		{
			kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
		}
		break;
	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
		break;
	case WM_CHAR:
		kbd.OnChar(static_cast<unsigned char>(wParam));
		break;
	case WM_INPUT:
		if (wParam == RIM_INPUT)
		{
			dword dwSize = 48;
			RAWINPUT raw;

			if (GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &raw, &dwSize,
				sizeof(RAWINPUTHEADER)) != dwSize)
				OutputDebugString(TEXT("GetRawInputData does not return correct size !\n"));

			if (raw.header.dwType == RIM_TYPEMOUSE)
			{
				this->mouse.TranslateMouseInput(raw.data.mouse);
			}
			break;
		}
	case WM_MOUSEMOVE:
		mouse.absX = GET_X_LPARAM(lParam);
		mouse.absY = GET_Y_LPARAM(lParam);
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