#include <Game/Window.h>
#include <sstream>
#include <resource.h>
#include <bindings/imgui_impl_win32.h>

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

enum class MenuItems :UINT_PTR
{
	Load = ID_FILE_LOADMODEL,
	Exit = ID_FILE_EXIT,
	ShowGrid = ID_OPTIONS_DRAWGRID,
	Style_VGUI = ID_STYLES_VGUI,
	Style_Dark = ID_STYLES_DARK,
	Style_Cherry = ID_STYLES_CHERRY,
	About = ID_HELP_ABOUT,
};

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

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
	wcWindow.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcWindow.hIcon = LoadIcon(wcWindow.hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wcWindow.hbrBackground = nullptr;
	wcWindow.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1);
	wcWindow.hIconSm = nullptr;
	wcWindow.lpszClassName = GetName();
	RegisterClassExA(&wcWindow);
}
Window::WindowClass::~WindowClass()
{
	UnregisterClass(wndClassName, GetInstance());
}
const char* Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}
HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

// Window namespace
Window::Window(unsigned int width, unsigned int height, const char* name) :width(width), height(height)
{
	RECT rWindow;
	rWindow.left = 100;
	rWindow.right = width + rWindow.left;
	rWindow.top = 100;
	rWindow.bottom = height + rWindow.top;
	// Automatic calculation of window height and width to client region
	WND_CALL_INFO(AdjustWindowRect(&rWindow, WS_OVERLAPPEDWINDOW, TRUE));

	hWnd.reset(CreateWindowA(
		WindowClass::GetName(), name,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		rWindow.right - rWindow.left,
		rWindow.bottom - rWindow.top,
		nullptr, nullptr,
		WindowClass::GetInstance(), this
	));

	// Error checks
	if (!hWnd) throw WND_LAST_EXCEPT();
	ShowWindow(hWnd.get(), SW_SHOWDEFAULT);

	Accelerator.reset(LoadAccelerators(WindowClass::GetInstance(), MAKEINTRESOURCE(IDR_ACCELERATOR1)));

	// Init GUI (only one window supported)
	WND_CALL_INFO(ImGui_ImplWin32_Init(hWnd.get()));

	// Create Graphics object
	pGfx = std::make_unique<Graphics>(hWnd.get(), width, height);

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
}
void Window::SetTitle(std::string_view title)
{
	if (!SetWindowText(hWnd.get(), title.data()))
	{
		throw WND_LAST_EXCEPT();
	}
}

bool Window::RestyleCalled() const noexcept
{
	return bRestyleIssued;
}
void Window::RestyleComplete() noexcept
{
	bRestyleIssued = false;
}
Window::Style Window::GetStyle() const noexcept
{
	return style;
}

UINT Window::GetWidth() const noexcept
{
	return width;
}
UINT Window::GetHeight() const noexcept
{
	return height;
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
	EnableMenuItem(FileMenu.get(), 0, MF_BYPOSITION | MF_ENABLED);
	bLoadCallIssued = false;
}

bool Window::ResizeCalled() const noexcept
{
	return bResizeIssued;
}
void Window::ResizeComplete() noexcept
{
	bResizeIssued = false;
}

bool Window::DrawGrid() const noexcept
{
	return bGridEnabled;
}
bool Window::IsActive() const noexcept
{
	return bActive;
}

void Window::ConfineCursor() noexcept
{
	RECT rect;
	GetClientRect(hWnd.get(), &rect);
	MapWindowPoints(hWnd.get(), nullptr, reinterpret_cast<POINT*>(&rect), 2);
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

std::optional<WPARAM> Window::ProcessMessages()const noexcept
{
	MSG msg;
	while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		if (!TranslateAccelerator(
			hWnd.get(),  // handle to receiving window 
			Accelerator.get(),    // handle to active accelerator table 
			&msg))         // message data 
		{
			if (msg.message == WM_QUIT)
			{
				return msg.wParam;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	if (!bActive)
	{
		WaitMessage();
	}
	return{};
}
Graphics& Window::Gfx()
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
	return DefWindowProc(hWnd, msg, wParam, lParam);
}
LRESULT WINAPI Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	// retrieve ptr to win class
	Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	// forward msg to class handler
	return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}
LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
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
	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			bActive = false;
		}
		if (wParam == SC_RESTORE)
		{
			bActive = true;
		}
		break;
	case WM_SIZE:
	{
		if (pGfx)
		{
			if (!LOWORD(lParam) || !HIWORD(lParam))
				break;
			width = LOWORD(lParam);
			height = HIWORD(lParam);
			bResizeIssued = true;
		}
		break;
	}
	case WM_CREATE:
		menu.reset(GetMenu(hWnd));
		FileMenu.reset(GetSubMenu(menu.get(), 0));
		OptionsMenu.reset(GetSubMenu(menu.get(), 1));
		StylesMenu.reset(GetSubMenu(menu.get(), 2));
		CheckMenuRadioItem(StylesMenu.get(), 0, 2, 0, MF_BYPOSITION);
		break;
	case WM_COMMAND:
		switch (MenuItems(LOWORD(wParam)))
		{
		case MenuItems::Load:
			EnableMenuItem(FileMenu.get(), 0, MF_BYPOSITION | MF_GRAYED);
			bLoadCallIssued = true;
			break;
		case MenuItems::Exit:
			PostQuitMessage(0);
			return 0;
		case MenuItems::ShowGrid:
		{
			bGridEnabled ^= 1;
			MENUITEMINFO mii
			{
				.cbSize = sizeof(MENUITEMINFO),
				.fMask = MIIM_STATE,
				.fState = UINT(bGridEnabled ? MFS_CHECKED : MFS_UNCHECKED)
			};
			SetMenuItemInfo(OptionsMenu.get(), 0, true, &mii);
			break;
		}
		case MenuItems::Style_VGUI:
			CheckMenuRadioItem(StylesMenu.get(), 0, 2, 0, MF_BYPOSITION);
			if (style != Style::VGUI)
			{
				style = Style::VGUI;
				bRestyleIssued = true;
			}

			break;
		case MenuItems::Style_Dark:
			CheckMenuRadioItem(StylesMenu.get(), 0, 2, 1, MF_BYPOSITION);
			if (style != Style::Dark)
			{
				style = Style::Dark;
				bRestyleIssued = true;
			}
			break;
		case MenuItems::Style_Cherry:
			CheckMenuRadioItem(StylesMenu.get(), 0, 2, 2, MF_BYPOSITION);
			if (style != Style::Cherry)
			{
				style = Style::Cherry;
				bRestyleIssued = true;
			}
			break;
		case MenuItems::About:
			DialogBox(WindowClass::GetInstance(), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, About);
			break;
		default:
			break;
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