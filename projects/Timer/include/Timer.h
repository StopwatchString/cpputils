#pragma once

#include <vector>
#include <chrono>
#include <limits>

class Timer
{
typedef std::pair<std::chrono::high_resolution_clock::time_point, std::chrono::high_resolution_clock::time_point> Timing;
typedef std::chrono::high_resolution_clock::time_point NanosecondTimePoint;
typedef std::chrono::high_resolution_clock::duration NanosecondDuration;
typedef std::chrono::duration<double> SecondDuration;

public:
	Timer(bool basicTimer = false);
	~Timer();
	void start();
	void stop();

	void writeToCSV(std::string fileName);
	double getMax() const		{ return _max_ms; }
	double getMin()	const		{ return _min_ms; }
	double getAverage()	const	{ return _average_ms; }

private:
	bool _basicTimer	{ false };
	double _max_ms		{ 0.0 };
	double _min_ms		{ std::numeric_limits<double>::max() };
	double _sum_ms		{ 0.0 };
	double _average_ms	{ 0.0 };

	NanosecondTimePoint startTime;
	NanosecondTimePoint endTime;

	std::vector<Timing> timings;
};