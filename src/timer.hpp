#pragma once

#include <chrono>
#include <string>
#include <iostream>

namespace art {
	class Timer final {
	public:
		Timer(const std::string &name = "Default") :
			m_startTimepoint(std::chrono::high_resolution_clock::now()),
			m_name(name),
			m_stopped(false) {}
		~Timer() { if (!m_stopped) Stop(); }

		void Stop() {
			m_stopped = true;

			auto endTime = std::chrono::high_resolution_clock::now();

			auto start = std::chrono::time_point_cast<std::chrono::milliseconds>(m_startTimepoint).time_since_epoch().count();
			auto end = std::chrono::time_point_cast<std::chrono::milliseconds>(endTime).time_since_epoch().count();

			auto duration = end - start;

			std::cout << "[Timer] " << m_name << " = " << duration * 0.001 << " seconds (" << duration << " ms)\n";
		}

	private:
		std::chrono::time_point<std::chrono::high_resolution_clock> m_startTimepoint;
		std::string m_name;
		bool m_stopped;
	};
}