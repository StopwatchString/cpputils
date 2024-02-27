#include "Timer.h"
#include <iostream>
#include <fstream>

Timer::Timer(bool basicTimer)
{
    _basicTimer = basicTimer;
    if (_basicTimer) {
        startTime = std::chrono::high_resolution_clock::now();
    }
}

Timer::~Timer()
{
    if (_basicTimer) {
        endTime = std::chrono::high_resolution_clock::now();
        NanosecondDuration duration = endTime - startTime;
        std::cout << "INFO Timer::~Timer() Time Elapsed: " << duration.count() / 1000 << "(ms)" << std::endl;
    }
}

void Timer::start()
{
    if (!_basicTimer) {
        startTime = std::chrono::high_resolution_clock::now();
    }
    else {
        std::cerr << "WARNING Timer::start() called while _basicTimer mode is enabled. Call ignored." << std::endl;
    }
}

void Timer::stop()
{
    if (!_basicTimer) {
        endTime = std::chrono::high_resolution_clock::now();
        timings.emplace_back(std::make_pair(startTime, endTime));

        NanosecondDuration duration = endTime - startTime;
        _sum_ms += duration.count() / 1000;
        _average_ms = _sum_ms / timings.size();
    }
    else {
        std::cerr << "WARNING Timer::stop() called while _basicTimer mode is enabled. Call ignored." << std::endl;
    }
}

void Timer::writeToCSV(std::string fileName) 
{
    std::cout << "WARNING Timer::writeToCSV() not implemented yet." << std::endl;
    return;

    std::ofstream outFile(fileName);
    if (outFile.is_open()) {
        for (Timing t : timings) {

        }
    }
}
