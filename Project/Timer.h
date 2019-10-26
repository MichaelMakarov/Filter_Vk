#pragma once

#include <chrono>
#include <iostream>


class Timer
{
	std::chrono::steady_clock::time_point start;
public:
	Timer();
	~Timer();
	void Start();
	void Finish();
};

