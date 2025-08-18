#ifndef CPPUTILS_RANGE_METER_H
#define CPPUTILS_RANGE_METER_H

#include <iostream>
#include <thread>
#include <chrono>
#include <cstdint>
#include <sstream>
#include <format>
#include <algorithm>
#include <vector>
#include <array>

#ifdef _WIN32
#include "cpputils/windows/terminal.h"
#endif

namespace cpputils {
namespace rangemeter {

#ifdef _WIN32
    constexpr size_t BUFFER_COUNT = 2;
    static HANDLE consoleScreenBuffer[BUFFER_COUNT]{ NULL };
    static size_t currentIndex{ 0 };
#endif

    constexpr const char* ANSI_RESET =        "\x1b[0m";
    constexpr size_t      ANSI_RESET_LEN =    5;
    constexpr const char* ANSI_GREEN =        "\x1b[32m";
    constexpr size_t      ANSI_GREEN_LEN =    6;
    constexpr const char* ANSI_YELLOW =       "\x1b[33m";
    constexpr size_t      ANSI_YELLOW_LEN =   6;
    constexpr const char* ANSI_RED =          "\x1b[31m";
    constexpr size_t      ANSI_RED_LEN =      6;
    constexpr const char* ANSI_BLUE =         "\x1b[34m";
    constexpr size_t      ANSI_BLUE_LEN =     6;

    constexpr const char* ANSI_HIDE_CURSOR =  "\x1b[?25l";
    constexpr size_t      ANSI_HIDE_CURSOR_LEN = 7;
    constexpr const char* ANSI_SHOW_CURSOR =  "\x1b[?25h";
    constexpr size_t      ANSI_SHOW_CURSOR_LEN = 7;
    constexpr const char* ANSI_CLEAR_AND_CR = "\x1b[2K\r";
    constexpr size_t      ANSI_CLEAR_AND_CR_LEN = 6;

struct RangeMeter
{
    std::string string{};

    std::string title;
    double min                      { 0.0 };
    double max                      { 0.0 };
    double current                  { 0.0 };
    double yellowPercent            { 0.4 };
    double redPercent               { 0.7 };
    double bluePercent              { 0.995 };

    // Drawing helpers
    size_t pipsOffset       { 0 };
    size_t pipsCount        { 50 };
    size_t greenPipsOffset  { 0 };
    size_t greenPipsCount   { 0 };
    size_t yellowPipsOffset { 0 };
    size_t yellowPipsCount  { 0 };
    size_t redPipsOffset    { 0 };
    size_t redPipsCount     { 0 };
    size_t bluePipsOffset   { 0 };
    size_t bluePipsCount    { 0 };

    size_t currentValOffset         { 0 };

    // Threading state
    size_t printThreadSleepMs       { 50 };
    bool printThreadRunFlag         { false };
    std::thread printThread         {};
};

size_t getRequiredCharCount(const double d)
{
    size_t count = 0;

    // Space for negative sign
    if (d < 0.0) count++;

    // Add one per significant digit
    uint64_t significantDigits = static_cast<uint64_t>(std::abs(d));

    if (significantDigits == 0) count++;

    while (significantDigits > 0) {
        significantDigits /= 10;
        count++;
    }

    // Add 1 for the decimal
    count++;

    // Add 2 for decimal places
    count += 2;

    return count;
}

void writeDoubleToStringWithOffset(std::string& target, size_t offset, const double d)
{
    size_t length = getRequiredCharCount(d);

    std::to_chars(target.data() + offset, target.data() + offset + length, d, std::chars_format::fixed, 2);
}

void buildString(RangeMeter& rangeMeter)
{
    // Get pip counts
    rangeMeter.greenPipsCount = std::round(rangeMeter.pipsCount * rangeMeter.yellowPercent);
    rangeMeter.yellowPipsCount = std::round(rangeMeter.pipsCount * (rangeMeter.redPercent - rangeMeter.yellowPercent));
    rangeMeter.redPipsCount = std::round(rangeMeter.pipsCount * (1.0 - rangeMeter.redPercent)) - 1;
    rangeMeter.bluePipsCount = 1;


    // Determine string size required
    size_t stringSize = 0;
    stringSize += rangeMeter.title.size();                  // Title
    stringSize += 1;                                        // ' '
    stringSize += getRequiredCharCount(rangeMeter.min);     // Number of slots for min
    stringSize += 1;                                        // '['
    stringSize += ANSI_GREEN_LEN;                           // len(ANSI_GREEN)
    stringSize += rangeMeter.greenPipsCount;                // Green pips
    stringSize += ANSI_YELLOW_LEN;                          // len(ANSI_YELLOW)
    stringSize += rangeMeter.yellowPipsCount;               // Yellow pips
    stringSize += ANSI_RED_LEN;                             // len(ANSI_RED)
    stringSize += rangeMeter.redPipsCount;                  // Red pips
    stringSize += ANSI_BLUE_LEN;                            // len(ANSI_BLUE)
    stringSize += rangeMeter.bluePipsCount;                 // Blue pips
    stringSize += ANSI_RESET_LEN;                           // len(ANSI_RESET)
    stringSize += 2;                                        // '] '
    stringSize += getRequiredCharCount(rangeMeter.max);     // Number of slots for max
    stringSize += 1;                                        // ' '
    stringSize += getRequiredCharCount(rangeMeter.max) + 6; // Assume (max+6) is a reasonable width for the current value.
                                                            // If you're exceeding by 6 orders of magnitude, we'll assume
                                                            // it's not our problem to display it correctly. (to_chars is safe)

    rangeMeter.string.clear();
    rangeMeter.string.resize(stringSize);

    // Fills the buffer with an obvious incorrect symbol for debugging
    memset(rangeMeter.string.data(), '+', stringSize);

    // Prep string
    size_t offset = 0;

    memcpy(rangeMeter.string.data(), rangeMeter.title.data(), rangeMeter.title.size());
    offset += rangeMeter.title.size();

    rangeMeter.string[offset] = ' ';
    offset++;

    writeDoubleToStringWithOffset(rangeMeter.string, offset, rangeMeter.min);
    offset += getRequiredCharCount(rangeMeter.min);

    rangeMeter.string[offset] = '[';
    offset++;

    rangeMeter.pipsOffset = offset;

    // Green
    memcpy(rangeMeter.string.data() + offset, ANSI_GREEN, ANSI_GREEN_LEN);
    offset += ANSI_GREEN_LEN;

    rangeMeter.greenPipsOffset = offset;
    memset(rangeMeter.string.data() + offset, ' ', rangeMeter.greenPipsCount);
    offset += rangeMeter.greenPipsCount;
    
    // Yellow
    memcpy(rangeMeter.string.data() + offset, ANSI_YELLOW, ANSI_YELLOW_LEN);
    offset += ANSI_YELLOW_LEN;

    rangeMeter.yellowPipsOffset = offset;
    memset(rangeMeter.string.data() + offset, ' ', rangeMeter.yellowPipsCount);
    offset += rangeMeter.yellowPipsCount;

    // Red
    memcpy(rangeMeter.string.data() + offset, ANSI_RED, ANSI_RED_LEN);
    offset += ANSI_RED_LEN;

    rangeMeter.redPipsOffset = offset;
    memset(rangeMeter.string.data() + offset, ' ', rangeMeter.redPipsCount);
    offset += rangeMeter.redPipsCount;

    // Blue
    memcpy(rangeMeter.string.data() + offset, ANSI_BLUE, ANSI_BLUE_LEN);
    offset += ANSI_BLUE_LEN;

    rangeMeter.bluePipsOffset = offset;
    memset(rangeMeter.string.data() + offset, ' ', rangeMeter.bluePipsCount);
    offset += rangeMeter.bluePipsCount;

    // Color Reset
    memcpy(rangeMeter.string.data() + offset, ANSI_RESET, ANSI_RESET_LEN);
    offset += ANSI_RESET_LEN;

    rangeMeter.string[offset] = ']';
    offset++;

    writeDoubleToStringWithOffset(rangeMeter.string, offset, rangeMeter.max);
    offset += getRequiredCharCount(rangeMeter.max);

    rangeMeter.string[offset] = ' ';
    offset++;

    rangeMeter.currentValOffset = offset;
    writeDoubleToStringWithOffset(rangeMeter.string, offset, rangeMeter.current);
    offset += getRequiredCharCount(rangeMeter.current);
}

void updateString(RangeMeter& rangeMeter)
{
    // Clear previous bar and current value readout
    memset(rangeMeter.string.data() + rangeMeter.greenPipsOffset,  ' ', rangeMeter.greenPipsCount);
    memset(rangeMeter.string.data() + rangeMeter.yellowPipsOffset, ' ', rangeMeter.yellowPipsCount);
    memset(rangeMeter.string.data() + rangeMeter.redPipsOffset,    ' ', rangeMeter.redPipsCount);
    memset(rangeMeter.string.data() + rangeMeter.bluePipsOffset,   ' ', rangeMeter.bluePipsCount);
    memset(rangeMeter.string.data() + rangeMeter.currentValOffset, ' ', rangeMeter.string.size() - rangeMeter.currentValOffset);

    const double clamped = std::clamp(rangeMeter.current, rangeMeter.min, rangeMeter.max);
    const double percent = (clamped - rangeMeter.min) / (rangeMeter.max - rangeMeter.min);

    auto getPercentInRange = [](const double percent, const double rangeMin, const double rangeMax)
        {
            const double clamped = std::clamp(percent, rangeMin, rangeMax);
            return (clamped - rangeMin) / (rangeMax - rangeMin);
        };

    int64_t filledGreen = rangeMeter.greenPipsCount * getPercentInRange(percent, 0.0, rangeMeter.yellowPercent);
    int64_t filledYellow = rangeMeter.yellowPipsCount * getPercentInRange(percent, rangeMeter.yellowPercent, rangeMeter.redPercent);
    int64_t filledRed = rangeMeter.redPipsCount * getPercentInRange(percent, rangeMeter.redPercent, rangeMeter.bluePercent);
    int64_t filledBlue = rangeMeter.bluePipsCount * getPercentInRange(percent, rangeMeter.bluePercent, 1.0);

    memset(rangeMeter.string.data() + rangeMeter.greenPipsOffset,  '=', filledGreen);
    memset(rangeMeter.string.data() + rangeMeter.yellowPipsOffset, '=', filledYellow);
    memset(rangeMeter.string.data() + rangeMeter.redPipsOffset,    '=', filledRed);
    memset(rangeMeter.string.data() + rangeMeter.bluePipsOffset,   '=', filledBlue);

    writeDoubleToStringWithOffset(rangeMeter.string, rangeMeter.currentValOffset, rangeMeter.current);
}

RangeMeter create(const std::string& title, const double min, const double max, const double yellowPercent = 0.4, const double redPercent = 0.7, const size_t pipsCount = 50)
{
#ifdef WIN32
    // Enable ANSI functionality in console
    if (!cpputils::windows::setConsoleVirtualTerminalProcessing(true)) {
        std::cout << "cpputils::RangeMeter createRangeMeter() Warning- Failed to enable Windows Virtual Terminal Processing!" << std::endl;
    }
    cpputils::windows::setConsoleEOLWrapping(false);

    // Create swap buffers to write to
    consoleScreenBuffer[0] = GetStdHandle(STD_OUTPUT_HANDLE);
    for (int i = 1; i < BUFFER_COUNT; i++) {
        consoleScreenBuffer[i] = CreateConsoleScreenBuffer(
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CONSOLE_TEXTMODE_BUFFER,
            NULL
        );
        WriteConsole(consoleScreenBuffer[i], ANSI_HIDE_CURSOR, ANSI_HIDE_CURSOR_LEN, NULL, NULL);
    }

#endif
    RangeMeter rangeMeter;

    rangeMeter.title = title;
    rangeMeter.min = min;
    rangeMeter.max = max;
    rangeMeter.yellowPercent = yellowPercent;
    rangeMeter.redPercent = redPercent;
    rangeMeter.pipsCount = pipsCount;

    buildString(rangeMeter);

    return rangeMeter;
}

void startThread(RangeMeter& rangeMeter)
{
    if (!rangeMeter.printThread.joinable()) {
        rangeMeter.printThreadRunFlag = true;
        rangeMeter.printThread = std::thread([&rangeMeter]() {
            std::cout << ANSI_HIDE_CURSOR;
            while (rangeMeter.printThreadRunFlag) {
                // Update range meter
                updateString(rangeMeter);

#ifdef _WIN32

                DWORD charsWritten;
                WriteConsole(consoleScreenBuffer[currentIndex], ANSI_CLEAR_AND_CR, ANSI_CLEAR_AND_CR_LEN, &charsWritten, NULL);

                WriteConsole(consoleScreenBuffer[currentIndex], rangeMeter.string.c_str(), rangeMeter.string.size(), &charsWritten, NULL);
                SetConsoleActiveScreenBuffer(consoleScreenBuffer[currentIndex]);

                currentIndex = (currentIndex + 1) % BUFFER_COUNT;
#else
                // Clear
                std::cout << ANSI_CLEAR_AND_CR;

                // Print range meter
                std::cout << rangeMeter.string;
#endif

                // Sleep
                std::this_thread::sleep_for(std::chrono::milliseconds(rangeMeter.printThreadSleepMs));
            }
            std::cout << ANSI_SHOW_CURSOR;
            });
    }
    else {
        std::cout << "cpputils::RangeMeter startRangeMeterThread() Warning- RangeMeter print thread is already running. Ignoring call." << std::endl;
    }
}

void stopThread(RangeMeter& rangeMeter)
{
    if (rangeMeter.printThread.joinable()) {
        rangeMeter.printThreadRunFlag = false;
        rangeMeter.printThread.join();
    }
}

} // End RangeMeter
} // End cpputils

#endif
