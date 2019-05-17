#include "Engine\Window.h"
#include <sstream>
int __stdcall WinMain(
	HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdShow
)
{
	try
	{
		Window wnd(1920, 1080, "Veritas 3D");

		MSG msg;
		BOOL gResult;
		while ((gResult = GetMessage(&msg, NULL, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			while (!wnd.mouse.IsEmpty())
			{
				const auto e = wnd.mouse.Read();
				if (e.GetType() == Mouse::Event::Type::Move)
				{
					std::ostringstream oss;
					oss << "Mouse pos delta: " << "(" << e.GetRelativeX() << ":" << e.GetRelativeY() << ")";
					wnd.SetTitle(oss.str());
				}
				if (e.GetType() == Mouse::Event::Type::MWDown)
				{
					std::ostringstream oss;
					oss << "Mouse wheel delta: " << "(" << e.GetDelta()  << ")";
					wnd.SetTitle(oss.str());
				}
				if (e.GetType() == Mouse::Event::Type::MWUp)
				{
					std::ostringstream oss;
					oss << "Mouse wheelup delta: " << "(" << e.GetDelta() << ")";
					wnd.SetTitle(oss.str());
				}
			}
		}
		if (gResult == -1)
		{
			return -1;
		}
		return msg.wParam;
	}
	catch (const Exception& e)
	{
		MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch(const std::exception& e)
	{
		MessageBox(nullptr, e.what(), "Standart Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr,"No Details on this one", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	return -1;
}