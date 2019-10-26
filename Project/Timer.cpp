#include "Timer.h"

Timer::Timer()
{
}

Timer::~Timer()
{
}

void Timer::Start()
{
	std::cout << "Start" << std::endl;
	start = std::chrono::high_resolution_clock::now();
}

void Timer::Finish()
{
	std::cout << "Finish: " << std::chrono::duration_cast<std::chrono::microseconds>(
		std::chrono::high_resolution_clock::now() - start).count()<< std::endl;
}