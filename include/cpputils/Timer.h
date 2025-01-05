#ifndef CPPUTILS_TIMER_H
#define CPPUTILS_TIMER_H

#include <chrono>
#include <iostream>

#if __cplusplus >= 202002L
#include <concepts>

template <typename T>
concept Clock = requires {
    typename T::time_point;
    { T::now() } -> std::convertible_to<typename T::time_point>;
};

template <typename T>
concept ChronoDuration =
std::same_as<T, std::chrono::nanoseconds> ||
std::same_as<T, std::chrono::microseconds> ||
std::same_as<T, std::chrono::milliseconds> ||
std::same_as<T, std::chrono::seconds> ||
std::same_as<T, std::chrono::minutes> ||
std::same_as<T, std::chrono::hours>;

//------------------------------------------------------------
// class ImmutableTimer
//------------------------------------------------------------
template <Clock clockType>
class ImmutableTimer {
public:
    ImmutableTimer() {
        startTime = clockType::now();
    }

    int64_t getElapsedTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clockType::now() - startTime).count();
    }

    int64_t getElapsedTimeNs() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clockType::now() - startTime).count();
    }

    template <ChronoDuration durationType>
    durationType getDuration() const {
        return std::chrono::duration_cast<durationType>(clockType::now() - startTime);
    }

private:
    clockType::time_point startTime;
};

//------------------------------------------------------------
// class ResettableTimer
//------------------------------------------------------------
template <Clock clockType>
class ResettableTimer {
public:
    ResettableTimer() {
        startTime = clockType::now();
    }

    void reset() {
        startTime = clockType::now();
    }

    int64_t getElapsedTimeMs() const {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clockType::now() - startTime).count();
    }

    int64_t getElapsedTimeNs() const {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clockType::now() - startTime).count();
    }

    template <ChronoDuration durationType>
    durationType getDuration() const {
        return std::chrono::duration_cast<durationType>(clockType::now() - startTime);
    }

private:
    clockType::time_point startTime;
};

//------------------------------------------------------------
// class ScopePrintTimer
//------------------------------------------------------------
template <Clock clockType, ChronoDuration durationType>
class ScopePrintTimer {
public:
    ScopePrintTimer(const std::string& printoutPrefix) : _printoutPrefix(printoutPrefix)
    {
        startTime = clockType::now();
    }

    ~ScopePrintTimer() {
        std::cout << _printoutPrefix << std::chrono::duration_cast<durationType>(clockType::now() - startTime).count() << std::endl;
    }

private:
    clockType::time_point startTime;
    const std::string _printoutPrefix;
};


#else

#include <type_traits>

//------------------------------------------------------------
// class Timer
//------------------------------------------------------------
template <typename T, typename = std::void_t<typename T::time_point, decltype(T::now())>>
class Timer
{
public:
    Timer() {
        startTime = T::now();
    }

private:
    typename T::time_point startTime;
};

#endif

#endif // End CPPUTILS_TIMER_H
