#pragma once
#include <chrono>
#include <format>

namespace ver
{
	class timer
	{
		using clock = std::chrono::high_resolution_clock;
	public:
		void start()
		{
			start_time = clock::now();
		}
		auto stop()
		{
			return std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start_time);
		}
	private:
		std::chrono::time_point<clock> start_time;
	};

	class scoped_timer
	{
	public:
		scoped_timer()
		{
			t.start();
		}
		~scoped_timer()
		{
			std::printf("%s", std::format("{}\n", t.stop()).c_str());
		}
	private:
		timer t;
	};
}