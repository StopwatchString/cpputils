#ifndef CPPUTILS_TIMER_H
#define CPPUTILS_TIMER_H

#define LANGUAGE_VERSION(version) (__cplusplus >= version || _MSVC_LANG >= version)

#include <chrono>
#include <iostream>
#include <array>

#if LANGUAGE_VERSION(202002L)
    #include <concepts>

namespace cpputils {

template<typename T>
concept Clock = requires {
    typename T::time_point;
    { T::now() } -> std::convertible_to<typename T::time_point>;
};

template<typename T>
concept ChronoDuration = std::same_as<T, std::chrono::nanoseconds> || std::same_as<T, std::chrono::microseconds>
                         || std::same_as<T, std::chrono::milliseconds> || std::same_as<T, std::chrono::seconds>
                         || std::same_as<T, std::chrono::minutes> || std::same_as<T, std::chrono::hours>;

template<typename T>
concept OutputStream = requires(T& stream, long long durationCount) {
    { stream << durationCount } -> std::same_as<T&>;
};

//------------------------------------------------------------
// class ImmutableTimer
//------------------------------------------------------------
template<Clock clockType>
class ImmutableTimer
{
public:
    ImmutableTimer() { startTime = clockType::now(); }

    int64_t getElapsedTimeMs() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clockType::now() - startTime).count();
    }

    int64_t getElapsedTimeNs() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clockType::now() - startTime).count();
    }

    template<ChronoDuration durationType>
    durationType getDuration() const
    {
        return std::chrono::duration_cast<durationType>(clockType::now() - startTime);
    }

private:
    clockType::time_point startTime;
};

//------------------------------------------------------------
// class ResettableTimer
//------------------------------------------------------------
template<Clock clockType>
class ResettableTimer
{
public:
    ResettableTimer() { startTime = clockType::now(); }

    void reset() { startTime = clockType::now(); }

    int64_t getElapsedTimeMs() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(clockType::now() - startTime).count();
    }

    int64_t getElapsedTimeNs() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clockType::now() - startTime).count();
    }

    template<ChronoDuration durationType>
    durationType getDuration() const
    {
        return std::chrono::duration_cast<durationType>(clockType::now() - startTime);
    }

private:
    clockType::time_point startTime;
};

//------------------------------------------------------------
// class StatsTimer
//------------------------------------------------------------
template<Clock clockType, size_t retainedTimingsSize>
class StatsTimer
{
public:
    StatsTimer() {}

    void reset()
    {
        timings.fill(std::chrono::nanoseconds(0));
        index = 0;
    }

    void start() { lastMeasuredTime = clockType::now(); }

    void stop()
    {
        std::chrono::nanoseconds elapsedTime
            = std::chrono::duration_cast<std::chrono::nanoseconds>(clockType::now() - lastMeasuredTime);
        timings[index] = elapsedTime;
        index = (index + 1) % retainedTimingsSize;
    }

    int64_t getAvgTimeMs()
    {
        int64_t totalTime = 0;
        for (const std::chrono::nanoseconds& timingNs : timings) {
            const std::chrono::milliseconds timingMs = std::chrono::duration_cast<std::chrono::milliseconds>(timingNs);
            totalTime += timingMs.count();
        }
        totalTime /= retainedTimingsSize;
        return totalTime;
    }

private:
    clockType::time_point lastMeasuredTime{};
    std::array<std::chrono::nanoseconds, retainedTimingsSize> timings;
    size_t index{0};
};

//------------------------------------------------------------
// class ScopePrintTimer
//------------------------------------------------------------
template<Clock clockType, ChronoDuration durationType, OutputStream outputStreamType = std::ostream>
class ScopePrintTimer
{
public:
    ScopePrintTimer(const std::string& printoutPrefix, outputStreamType& ostream = std::cout) :
        _printoutPrefix(printoutPrefix), _ostream(ostream)
    {
        startTime = clockType::now();
    }

    ~ScopePrintTimer()
    {
        _ostream << _printoutPrefix << std::chrono::duration_cast<durationType>(clockType::now() - startTime).count()
                 << std::endl;
    }

private:
    clockType::time_point startTime;
    outputStreamType& _ostream;
    const std::string _printoutPrefix;
};

//------------------------------------------------------------
// class FramerateTimer
//------------------------------------------------------------
template<Clock clockType, size_t sampleCount>
class FramerateTimer
{
public:
    FramerateTimer()
        : timingsSum(std::chrono::nanoseconds(0)),
          index(0)
    {
        lastMeasuredTime = clockType::now();
    }

    void reset()
    {
        lastMeasuredTime = clockType::now();
        timings.fill(std::chrono::nanoseconds(0));
    }

    void markFrame()
    {
        typename clockType::time_point frameTime = clockType::now();
        std::chrono::nanoseconds elapsedTime = std::chrono::duration_cast<std::chrono::nanoseconds>(frameTime - lastMeasuredTime);

        timingsSum -= timings[index];
        timings[index] = elapsedTime;
        timingsSum += timings[index];
        index = (index + 1) % sampleCount;

        lastMeasuredTime = frameTime;
    }

    double getFrametimeMs() const
    {
        const double avgTimingNs = (double)timingsSum.count() / sampleCount;
        const double avgTimingMs = avgTimingNs / 1'000'000;
        return avgTimingMs;
    }

    double getFramerate() const
    {
        const double avgTimingNs = (double)timingsSum.count() / sampleCount;
        const double avgTimingS = avgTimingNs / 1'000'000'000;
        return 1 / avgTimingS;
    }

private:
    clockType::time_point lastMeasuredTime{};
    std::array<std::chrono::nanoseconds, sampleCount> timings{std::chrono::nanoseconds(0)};
    size_t index{ 0 };
    std::chrono::nanoseconds timingsSum;
};

} // namespace cpputils

#else

    #include <type_traits>

namespace cpputils {

//------------------------------------------------------------
// Type Traits for Constraints
//------------------------------------------------------------

// Trait to check if a type is a valid Clock
template<typename T, typename = void>
struct is_clock : std::false_type
{};

template<typename T>
struct is_clock<T, std::void_t<typename T::time_point, decltype(T::now())>> : std::true_type
{};

// Trait to check if a type is a valid Chrono Duration
template<typename T>
struct is_chrono_duration : std::false_type
{};

template<>
struct is_chrono_duration<std::chrono::nanoseconds> : std::true_type
{};

template<>
struct is_chrono_duration<std::chrono::microseconds> : std::true_type
{};

template<>
struct is_chrono_duration<std::chrono::milliseconds> : std::true_type
{};

template<>
struct is_chrono_duration<std::chrono::seconds> : std::true_type
{};

template<>
struct is_chrono_duration<std::chrono::minutes> : std::true_type
{};

template<>
struct is_chrono_duration<std::chrono::hours> : std::true_type
{};

// Trait to check if a type is a valid Output Stream
template<typename T, typename = void>
struct is_output_stream : std::false_type
{};

template<typename T>
struct is_output_stream<T, std::void_t<decltype(std::declval<T&>() << std::declval<long long>())>> : std::true_type
{};

//------------------------------------------------------------
// class ImmutableTimer
//------------------------------------------------------------
template<typename ClockType, typename std::enable_if<is_clock<ClockType>::value, int>::type = 0>
class ImmutableTimer
{
public:
    ImmutableTimer() { startTime = ClockType::now(); }

    int64_t getElapsedTimeMs() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(ClockType::now() - startTime).count();
    }

    int64_t getElapsedTimeNs() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(ClockType::now() - startTime).count();
    }

    template<typename DurationType, typename std::enable_if<is_chrono_duration<DurationType>::value, int>::type = 0>
    DurationType getDuration() const
    {
        return std::chrono::duration_cast<DurationType>(ClockType::now() - startTime);
    }

private:
    typename ClockType::time_point startTime;
};

//------------------------------------------------------------
// class ResettableTimer
//------------------------------------------------------------
template<typename ClockType, typename std::enable_if<is_clock<ClockType>::value, int>::type = 0>
class ResettableTimer
{
public:
    ResettableTimer() { startTime = ClockType::now(); }

    void reset() { startTime = ClockType::now(); }

    int64_t getElapsedTimeMs() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(ClockType::now() - startTime).count();
    }

    int64_t getElapsedTimeNs() const
    {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(ClockType::now() - startTime).count();
    }

    template<typename DurationType, typename std::enable_if<is_chrono_duration<DurationType>::value, int>::type = 0>
    DurationType getDuration() const
    {
        return std::chrono::duration_cast<DurationType>(ClockType::now() - startTime);
    }

private:
    typename ClockType::time_point startTime;
};

//------------------------------------------------------------
// class ScopePrintTimer
//------------------------------------------------------------
template<
    typename ClockType,
    typename DurationType,
    typename OutputStreamType = std::ostream,
    typename std::enable_if<
        is_clock<ClockType>::value && is_chrono_duration<DurationType>::value
            && is_output_stream<OutputStreamType>::value,
        int>::type
    = 0>
class ScopePrintTimer
{
public:
    ScopePrintTimer(const std::string& printoutPrefix, OutputStreamType& ostream = std::cout) :
        _printoutPrefix(printoutPrefix), _ostream(ostream)
    {
        startTime = ClockType::now();
    }

    ~ScopePrintTimer()
    {
        _ostream << _printoutPrefix << std::chrono::duration_cast<DurationType>(ClockType::now() - startTime).count()
                 << std::endl;
    }

private:
    typename ClockType::time_point startTime;
    OutputStreamType& _ostream;
    const std::string _printoutPrefix;
};

} // namespace cpputils

#endif

#endif // End CPPUTILS_TIMER_H
