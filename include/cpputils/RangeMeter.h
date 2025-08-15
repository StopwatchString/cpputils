#ifndef CPPUTILS_RANGE_METER_H
#define CPPUTILS_RANGE_METER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <format>
#include <algorithm>

#ifdef WIN32
    #define NOMINMAX
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    // Windows command prompt requires special init to enable colors in the terminal.
    bool enableCommandPromptANSIColors() {
        static bool windowsANSIColorsInit = false;
        if (!windowsANSIColorsInit) {
            HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hOut == INVALID_HANDLE_VALUE) return false;

            DWORD dwMode = 0;
            if (!GetConsoleMode(hOut, &dwMode)) return false;

            // Enable the virtual terminal processing flag
            dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            windowsANSIColorsInit = SetConsoleMode(hOut, dwMode);
        }
        return windowsANSIColorsInit;
    }
#endif

namespace cpputils {

    constexpr const char* ANSI_RESET =  "\x1b[0m";
    constexpr const char* ANSI_GREEN =  "\x1b[32m";
    constexpr const char* ANSI_YELLOW = "\x1b[33m";
    constexpr const char* ANSI_RED =    "\x1b[31m";
    constexpr const char* ANSI_BLUE =   "\x1b[34m";

struct RangeMeter
{
    std::string title;

    double min                  { 0.0 };
    double max                  { 0.0 };
    double current              { 0.0 };
    double cautionPercent       { 0.4 };
    double warningPercent       { 0.7 };

    int64_t rangeBarPips         { 50 };
    std::stringstream ss;
    size_t printThreadSleepMs   { 100 };
    bool printThreadRunFlag     { false };
    std::thread printThread;
};

void printRangeMeter(RangeMeter& rangeMeter)
{
    // For the purposes of printing, we want to clamp the value so the bar can't be weird sizes
    const double evaluatedCurrent = std::clamp(rangeMeter.current, rangeMeter.min, rangeMeter.max);
    const double currentPercent = (evaluatedCurrent - rangeMeter.min)/(rangeMeter.max - rangeMeter.min);

    auto getPercentInRange = [](const double percent, const double rangeMin, const double rangeMax)
        {
            const double val = std::clamp(percent, rangeMin, rangeMax);
            return val - rangeMin;
        };

    const double greenPercent = getPercentInRange(currentPercent, 0.0, rangeMeter.cautionPercent);
    const double yellowPercent = getPercentInRange(currentPercent, rangeMeter.cautionPercent, rangeMeter.warningPercent);
    const double redPercent = getPercentInRange(currentPercent, rangeMeter.warningPercent, 1.0);

    int64_t filledGreen = greenPercent * rangeMeter.rangeBarPips;
    int64_t filledYellow = yellowPercent * rangeMeter.rangeBarPips;
    int64_t filledRed = redPercent * rangeMeter.rangeBarPips;
    int64_t filledBlue = 0;

    const int64_t totalFilled = filledGreen + filledYellow + filledRed;

    int64_t empty = totalFilled > rangeMeter.rangeBarPips ? 0 : rangeMeter.rangeBarPips - totalFilled;

    // If we're at least half a percent away from max, then color
    // the last pip blue. Otherwise due to truncation it will
    // forever be a space.
    if (currentPercent > (1.0 - 0.005)) {
        if (empty > 0) {
            empty = 0;
        }
        if (totalFilled == rangeMeter.rangeBarPips) {
            if (filledRed > 0) {
                filledRed--;
            }
            else if (filledYellow > 0) {
                filledYellow--;
            }
            else {
                filledGreen--;
            }
        }

        filledBlue = 1;
    }

    rangeMeter.ss.clear();
    rangeMeter.ss
        << '\r'
        << rangeMeter.title << " "
        << std::format("{:6.2f}", rangeMeter.current)
        << " "
        << std::format("{:3.2f}", rangeMeter.min)
        << "["
        << ANSI_GREEN << std::string(filledGreen, '=')
        << ANSI_YELLOW << std::string(filledYellow, '=')
        << ANSI_RED << std::string(filledRed, '=')
        << ANSI_BLUE << std::string(filledBlue, '=')
        << ANSI_RESET
        << std::string(empty, ' ')
        << "]"
        << std::format("{:3.2f}", rangeMeter.max);

    std::cout << rangeMeter.ss.str();
}

RangeMeter createRangeMeter(const std::string_view title, const double min, const double max, const double cautionPercent = 0.4, const double warningPercent = 0.7)
{
#ifdef WIN32
    if (!enableCommandPromptANSIColors()) {
        std::cout << "cpputils::RangeMeter createRangeMeter() Warning- Failed to enable Windows CommandPrompt ANSI color output!" << std::endl;
    }
#endif

    RangeMeter rangeMeter{};

    rangeMeter.title = title;
    rangeMeter.min = min;
    rangeMeter.max = max;
    rangeMeter.cautionPercent = cautionPercent;
    rangeMeter.warningPercent = warningPercent;

    return rangeMeter;
}

void startRangeMeterThread(RangeMeter& rangeMeter)
{
    if (!rangeMeter.printThread.joinable()) {
        rangeMeter.printThreadRunFlag = true;
        rangeMeter.printThread = std::thread([&rangeMeter](){
            while (rangeMeter.printThreadRunFlag) {
                printRangeMeter(rangeMeter);
                std::this_thread::sleep_for(std::chrono::milliseconds(rangeMeter.printThreadSleepMs));
            }
        });
    }
    else {
        std::cout << "cpputils::RangeMeter startRangeMeterThread() Warning- RangeMeter print thread is already running. Ignoring call." << std::endl;
    }
}

void stopRangeMeterThread(RangeMeter& rangeMeter)
{
    if (rangeMeter.printThread.joinable()) {
        rangeMeter.printThreadRunFlag = false;
        rangeMeter.printThread.join();
    }
}

} // End cpputils

#endif
