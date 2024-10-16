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
		float stop()
		{
			return (clock::now() - start_time).count() / 1'000'000'000.0f;
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