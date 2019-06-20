#pragma once
#include <queue>
#include <bitset>

class Keyboard
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
			Invalid
		};
	private:
		Type type;
		unsigned char code;
	public:
		Event()noexcept :type(Type::Invalid),code(0u){} // Fast c-tor
		Event(Type type, unsigned char code)noexcept:type(type),code(code){}
		bool IsPress()const noexcept { return type == Type::Press; }
		bool IsRelease()const noexcept { return type == Type::Release; }
		bool IsInvalid()const noexcept { return type == Type::Invalid; }
		unsigned char GetCode()const noexcept 
		{
			return code;
		}
	};
public:
	Keyboard() = default;
	Keyboard(const Keyboard&) = delete;
	Keyboard& operator=(const Keyboard&) = delete;
	// Key Events
	bool KeyPressed(unsigned char keycode)const noexcept;
	Event ReadKey() noexcept;
	bool KeyIsEmpty()const noexcept;
	void ClearKey()noexcept;
	// CharRoutines
	unsigned char ReadChar() noexcept;
	bool CharIsEmpty()const noexcept;
	void ClearChar()noexcept;
	void Flush()noexcept;
	// Autorepeat
private:
	void OnKeyPressed(unsigned char keycode)noexcept;
	void OnKeyReleased(unsigned char keycode)noexcept;
	void OnChar(char character)noexcept;
	void ClearState()noexcept;
private:
	static constexpr unsigned int nKeys = 256u;
	static constexpr unsigned int bufferSize = 16u;
	std::bitset<nKeys> KeyStates;
	std::queue<Event> KeyBuffer;
	std::queue<char> CharBuffer;
};

template<typename T>
inline void TrimBuffer(std::queue<T>& buffer, unsigned size) noexcept
{
	while (buffer.size() > size)
		buffer.pop();
}
