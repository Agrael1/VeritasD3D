#include "Mouse.h"


template<typename T>
inline void TrimBuffer(std::queue<T>& buffer, unsigned size) noexcept
{
	while (buffer.size() > size)
		buffer.pop();
}

Mouse::Event::Event()noexcept
{
	this->code = 0;
	this->type = Type::Invalid;
	this->delta = 0;
	this->dX = dY = 0;
}
Mouse::Event::Event(Type type, byte code)noexcept
{
	this->code = code;
	this->type = type;
	this->delta = 0;
	this->dX = dY = 0;
}
Mouse::Event::Event(short delta)noexcept
{
	this->code = 0;
	this->delta = delta;

	if (delta > 0)
		this->type = Type::MWUp;
	else
		this->type = Type::MWDown;
}
Mouse::Event::Event(long dx, long dy)noexcept
{
	this->code = 0;
	this->type = Type::Move;
	this->delta = 0;
	this->dX = dx;
	this->dY = dy;
}

Mouse::Event::Type Mouse::Event::GetType() const noexcept
{
	return this->type;
}

bool Mouse::Event::IsInvalid()const noexcept
{
	return this->type == Type::Invalid;
}
byte Mouse::Event::GetCode()const noexcept
{
	return this->code;
}
int  Mouse::Event::GetDelta()const noexcept
{
	return this->delta;
}

long Mouse::Event::GetRelativeX() const noexcept
{
	return dX;
}
long Mouse::Event::GetRelativeY() const noexcept
{
	return dY;
}
/*================================*/

Mouse::Event Mouse::Read() noexcept
{
	if (MouseBuffer.size() > 0u)
	{
		Mouse::Event e = MouseBuffer.front();
		MouseBuffer.pop();
		return e;
	}
	else
	{
		return Mouse::Event();
	}
}

POINT Mouse::GetAbsolute()const noexcept
{
	return {absX,absY};
}
bool Mouse::ButtonPressed(byte code) const noexcept
{
	return this->MBStates[code];
}
bool Mouse::IsEmpty() const noexcept
{
	return this->MouseBuffer.empty();
}
void Mouse::InitializeMouse(const HWND hWnd) noexcept
{
	Rid.usUsagePage = HID_USAGE_PAGE_GENERIC;
	Rid.usUsage = HID_USAGE_GENERIC_MOUSE;
	Rid.dwFlags = RIDEV_INPUTSINK /*| RIDEV_NOLEGACY*/;
	Rid.hwndTarget = hWnd;
	RegisterRawInputDevices(&Rid, 1, sizeof(RAWINPUTDEVICE));
}
void Mouse::TranslateMouseInput(const RAWMOUSE & mouse) noexcept
{
	auto _local = mouse.usButtonFlags;
	if ((mouse.usFlags == 0)&&(mouse.lLastX||mouse.lLastY))
	{
		MouseBuffer.push(Mouse::Event(mouse.lLastX, mouse.lLastY));
		TrimBuffer(MouseBuffer, bufferSize);
	}
	if (_local & RI_MOUSE_WHEEL)
	{
		MouseBuffer.push(Mouse::Event((short)mouse.usButtonData));
		TrimBuffer(MouseBuffer, bufferSize);
	}
	if (_local)
	{
		OnButtonPress(mouse.usButtonFlags);
	}
	
}
inline void Mouse::OnButtonPress(USHORT buttons)noexcept
{
	for (byte i = 0; i < 5; i++)
	{
		if (buttons & (1 << 2 * i))
		{
			MouseBuffer.push(Mouse::Event(Mouse::Event::Type::Press, i));
			MBStates[i] = true;
			TrimBuffer(MouseBuffer, bufferSize);
		}
		if (buttons & (1 << (2 * i + 1)))
		{
			MouseBuffer.push(Mouse::Event(Mouse::Event::Type::Release, i));
			MBStates[i] = false;
			TrimBuffer(MouseBuffer, bufferSize);
		}
	}

}
