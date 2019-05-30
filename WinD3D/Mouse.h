#pragma once
#include <queue>
#include <bitset>
#include "Types.h"
#include "Engine\WinSetup.h"

#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		enum class Type
		{
			Press,
			Release,
			Invalid,
			Move,
			MWUp,
			MWDown
		};
	private:
		Type type;
		byte code;
		long dX, dY;
		short delta;
	public:
		Event()noexcept;
		Event(Type type, byte code)noexcept;
		Event(short delta)noexcept;
		Event(long dx, long dy)noexcept;

		Type GetType()const noexcept;
		bool IsInvalid()const noexcept;
		byte GetCode()const noexcept;
		int GetDelta()const noexcept;
		long GetRelativeX()const noexcept;
		long GetRelativeY()const noexcept;
	};

public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
public:
	Mouse::Event Read() noexcept;
	POINT GetAbsolute() const noexcept;
	bool ButtonPressed(byte code)const noexcept;
	bool IsEmpty()const noexcept;
private:
	void InitializeMouse(const HWND hWnd)noexcept;
	void TranslateMouseInput(const RAWMOUSE& mouse)noexcept;
	void OnButtonPress(USHORT buttons)noexcept;
private:
	static constexpr unsigned char nKeys = 5u;
	static constexpr unsigned int bufferSize = 16u;

	int absX = 0, absY = 0;

	RAWINPUTDEVICE Rid;
	std::queue<Event> MouseBuffer;
	std::bitset<nKeys> MBStates;
};