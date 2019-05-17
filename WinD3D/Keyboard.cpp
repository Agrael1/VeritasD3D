#include "Engine\Keyboard.h"

bool Keyboard::KeyPressed(unsigned char keycode) const noexcept
{
	return KeyStates[keycode];
}
Keyboard::Event Keyboard::ReadKey() noexcept
{
	if (KeyBuffer.size() > 0u)
	{
		Keyboard::Event e = KeyBuffer.front();
		KeyBuffer.pop();
		return e;
	}
	else
	{
		return Keyboard::Event();
	}
}
bool Keyboard::KeyIsEmpty() const noexcept
{
	return KeyBuffer.empty();
}
void Keyboard::ClearKey() noexcept
{
	KeyBuffer = std::queue<Event>();
}

unsigned char Keyboard::ReadChar() noexcept
{
	if (CharBuffer.size() > 0u)
	{
		unsigned char c = CharBuffer.front();
		KeyBuffer.pop();
		return c;
	}
	else
	{
		return 0;
	}
}
bool Keyboard::CharIsEmpty() const noexcept
{
	return CharBuffer.empty();
}
void Keyboard::ClearChar() noexcept
{
	CharBuffer = std::queue<char>();
}
void Keyboard::Flush() noexcept
{
	ClearChar();
	ClearKey();
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept
{
	KeyStates[keycode] = true;
	KeyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(KeyBuffer, bufferSize);
}
void Keyboard::OnKeyReleased(unsigned char keycode) noexcept
{
	KeyStates[keycode] = false;
	KeyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(KeyBuffer, bufferSize);
}
void Keyboard::OnChar(char character) noexcept
{
	CharBuffer.push(character);
	TrimBuffer(CharBuffer, bufferSize);
}
void Keyboard::ClearState() noexcept
{
	KeyStates.reset();
}

