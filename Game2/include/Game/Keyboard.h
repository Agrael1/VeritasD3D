#pragma once
#include <queue>
#include <bitset>
#include <optional>

namespace ver
{
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
			};
		private:
			Type type;
			unsigned char code;
		public:
			Event(Type type, unsigned char code) noexcept
				:
				type(type),
				code(code)
			{}
			bool IsPress() const noexcept
			{
				return type == Type::Press;
			}
			bool IsRelease() const noexcept
			{
				return type == Type::Release;
			}
			unsigned char GetCode() const noexcept
			{
				return code;
			}
		};
	public:
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;
		// key event stuff
		bool KeyIsPressed(unsigned char keycode) const noexcept
		{
			return keystates[keycode];
		}
		std::optional<Event> ReadKey() noexcept
		{
			if (keybuffer.size() > 0u)
			{
				Event e = keybuffer.front();
				keybuffer.pop();
				return e;
			}
			return{};
		}
		bool KeyIsEmpty() const noexcept
		{
			return keybuffer.empty();
		}
		void FlushKey() noexcept
		{
			keybuffer = std::queue<Event>();
		}
		// char event stuff
		std::optional<char> ReadChar() noexcept
		{
			if (charbuffer.size() > 0u)
			{
				unsigned char charcode = charbuffer.front();
				charbuffer.pop();
				return charcode;
			}
			return{};
		}
		bool CharIsEmpty() const noexcept
		{
			return charbuffer.empty();
		}
		void FlushChar() noexcept
		{
			charbuffer = std::queue<char>();
		}
		void Flush() noexcept
		{
			FlushKey();
			FlushChar();
		}
		// autorepeat control
		void EnableAutorepeat() noexcept
		{
			autorepeatEnabled = true;
		}
		void DisableAutorepeat() noexcept
		{
			autorepeatEnabled = false;
		}
		bool AutorepeatIsEnabled() const noexcept
		{
			return autorepeatEnabled;
		}
	private:
		void OnKeyPressed(unsigned char keycode) noexcept
		{
			keystates[keycode] = true;
			keybuffer.push(ver::Keyboard::Event(ver::Keyboard::Event::Type::Press, keycode));
			TrimBuffer(keybuffer);
		}
		void OnKeyReleased(unsigned char keycode) noexcept
		{
			keystates[keycode] = false;
			keybuffer.push(ver::Keyboard::Event(ver::Keyboard::Event::Type::Release, keycode));
			TrimBuffer(keybuffer);
		}
		void OnChar(char character) noexcept
		{
			charbuffer.push(character);
			TrimBuffer(charbuffer);
		}
		void ClearState() noexcept
		{
			keystates.reset();
		}
		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) noexcept
		{
			while (buffer.size() > bufferSize)
			{
				buffer.pop();
			}
		}
	private:
		static constexpr unsigned int nKeys = 256u;
		static constexpr unsigned int bufferSize = 16u;
		bool autorepeatEnabled = false;
		std::bitset<nKeys> keystates;
		std::queue<Event> keybuffer;
		std::queue<char> charbuffer;
	};
}