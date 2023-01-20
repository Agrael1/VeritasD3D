#pragma once
#include <queue>
#include <optional>


namespace ver
{
	class Mouse
	{
		friend class Window;
	public:
		struct RawDelta
		{
			int x, y;
		};
		class Event
		{
		public:
			enum class Type
			{
				LPress,
				LRelease,
				RPress,
				RRelease,
				WheelUp,
				WheelDown,
				Move,
				Enter,
				Leave,
			};
		private:
			Type type;
			bool leftIsPressed;
			bool rightIsPressed;
			int x;
			int y;
		public:
			Event(Type type, const Mouse& parent) noexcept
				:
				type(type),
				leftIsPressed(parent.leftIsPressed),
				rightIsPressed(parent.rightIsPressed),
				x(parent.x),
				y(parent.y)
			{}
			Type GetType() const noexcept
			{
				return type;
			}
			std::pair<int, int> GetPos() const noexcept
			{
				return{ x,y };
			}
			int GetPosX() const noexcept
			{
				return x;
			}
			int GetPosY() const noexcept
			{
				return y;
			}
			bool LeftIsPressed() const noexcept
			{
				return leftIsPressed;
			}
			bool RightIsPressed() const noexcept
			{
				return rightIsPressed;
			}
		};
	public:
		Mouse() = default;
		Mouse(const Mouse&) = delete;
		Mouse& operator=(const Mouse&) = delete;
		std::pair<int, int> GetPos() const noexcept
		{
			return{ x,y };
		}
		std::optional<RawDelta> ReadRawDelta() noexcept
		{
			if (rawDeltaBuffer.empty())
			{
				return{};
			}
			const RawDelta d = rawDeltaBuffer.front();
			rawDeltaBuffer.pop();
			return d;
		}
		int GetPosX() const noexcept
		{
			return x;
		}
		int GetPosY() const noexcept
		{
			return y;
		}
		bool IsInWindow() const noexcept
		{
			return isInWindow;
		}
		bool LeftIsPressed() const noexcept
		{
			return leftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return rightIsPressed;
		}
		std::optional<Mouse::Event> Read() noexcept
		{
			if (buffer.size() > 0u)
			{
				ver::Mouse::Event e = buffer.front();
				buffer.pop();
				return e;
			}
			return{};
		}
		bool IsEmpty() const noexcept
		{
			return buffer.empty();
		}
		void Flush() noexcept
		{
			buffer = std::queue<Event>();
		}
		void EnableRaw() noexcept
		{
			rawEnabled = true;
		}
		void DisableRaw() noexcept
		{
			rawEnabled = false;
		}
		bool RawEnabled() const noexcept
		{
			return rawEnabled;
		}
	private:
		void OnMouseMove(int newx, int newy) noexcept
		{
			x = newx;
			y = newy;

			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::Move, *this));
			TrimBuffer();
		}
		void OnMouseLeave() noexcept
		{
			isInWindow = false;
			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::Leave, *this));
			TrimBuffer();
		}
		void OnMouseEnter() noexcept
		{
			isInWindow = true;
			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::Enter, *this));
			TrimBuffer();
		}
		void OnRawDelta(int dx, int dy) noexcept
		{
			rawDeltaBuffer.push({ dx,dy });
			TrimBuffer();
		}
		void OnLeftPressed(int x, int y) noexcept
		{
			leftIsPressed = true;

			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::LPress, *this));
			TrimBuffer();
		}
		void OnLeftReleased(int x, int y) noexcept
		{
			leftIsPressed = false;

			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::LRelease, *this));
			TrimBuffer();
		}
		void OnRightPressed(int x, int y) noexcept
		{
			rightIsPressed = true;

			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::RPress, *this));
			TrimBuffer();
		}
		void OnRightReleased(int x, int y) noexcept
		{
			rightIsPressed = false;

			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::RRelease, *this));
			TrimBuffer();
		}
		void OnWheelUp(int x, int y) noexcept
		{
			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::WheelUp, *this));
			TrimBuffer();
		}
		void OnWheelDown(int x, int y) noexcept
		{
			buffer.push(ver::Mouse::Event(ver::Mouse::Event::Type::WheelDown, *this));
			TrimBuffer();
		}
		void TrimBuffer() noexcept
		{
			while (buffer.size() > bufferSize)
			{
				buffer.pop();
			}
		}
		void TrimRawInputBuffer() noexcept
		{
			while (rawDeltaBuffer.size() > bufferSize)
			{
				rawDeltaBuffer.pop();
			}
		}
		void OnWheelDelta(int x, int y, int delta) noexcept
		{
			wheelDeltaCarry += delta;
			// generate events for every 120 
			while (wheelDeltaCarry >= wheelDelta)
			{
				wheelDeltaCarry -= wheelDelta;
				OnWheelUp(x, y);
			}
			while (wheelDeltaCarry <= -wheelDelta)
			{
				wheelDeltaCarry += wheelDelta;
				OnWheelDown(x, y);
			}
		}
	private:
		static constexpr unsigned int bufferSize = 16u;
		static constexpr unsigned int wheelDelta = 120u;
		int x;
		int y;
		bool leftIsPressed = false;
		bool rightIsPressed = false;
		bool isInWindow = false;
		int wheelDeltaCarry = 0;
		bool rawEnabled = false;
		std::queue<Event> buffer;
		std::queue<RawDelta> rawDeltaBuffer;
	};
}