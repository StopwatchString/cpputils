#include <vector>
#include <chrono>
#include <fstream>
#include <limits>

class Timer
{
public:
    Timer() = default;
    Timer(int sampleWindowSize)
    {
        _sampleWindowSize = sampleWindowSize;
    }

    double getAverage()     { return _avg; }
    double getLast()        { return _last; }
    double getMax()         { return _max; }
    double getMin()         { return _min; }
    double getSampleCount() { return _samples.size(); }

    void start()
    {
        _start = std::chrono::high_resolution_clock::now();
    }

    void stop()
    {
        std::chrono::high_resolution_clock::time_point _end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsedTime = _end - _start;
        double milliseconds = elapsedTime.count() * 1000;
        
        _last = milliseconds;
        _samples.push_back(_last);

        if (_samples.size() > _sampleWindowSize) {
            _sum -= _samples[_samples.size() - _sampleWindowSize - 1];
            _sum += _last;
            _avg = _sum / _sampleWindowSize;
        }
        else {
            _sum += _last;
            _avg = _sum / _samples.size();
        }

        int startIndex = 0;
        if (_samples.size() > _sampleWindowSize) {
            startIndex = _samples.size() - _sampleWindowSize;
        }

        _min = std::numeric_limits<double>::max();
        _max = std::numeric_limits<double>::min();

        for (int i = startIndex; i < _samples.size(); i++)
        {
            if (_samples[i] < _min) _min = _samples[i];
            if (_samples[i] > _max) _max = _samples[i];
        }
    }

    void clear()
    {
        _samples.clear();
        _last = 0.0;
        _sum = 0.0;
        _avg = 0.0;
        _min = 0.0;
        _max = 0.0;
    }

    void setSampleWindowSize(int sampleWindowSize)
    {
        _sampleWindowSize = sampleWindowSize;

        _sum = 0.0;
        int startIndex = 0;
        if (_samples.size() > _sampleWindowSize) {
            startIndex = _samples.size() - _sampleWindowSize;
        }

        for (int i = startIndex; i < _samples.size(); i++) {
            _sum += _samples[i];
        }
        
        _avg = _sum / (_samples.size() - startIndex);

        _min = std::numeric_limits<double>::max();
        _max = std::numeric_limits<double>::min();

        for (int i = startIndex; i < _samples.size(); i++)
        {
            if (_samples[i] < _min) _min = _samples[i];
            if (_samples[i] > _max) _max = _samples[i];
        }
    }

    void printToCSV(std::string filename, bool appendDateTime = true)
    {
        if (appendDateTime) {
            std::time_t now = std::time(nullptr);
            std::tm localTime = *std::localtime(&now);
            std::stringstream ss;
            ss << filename << " " << localTime.tm_mon << "-" << localTime.tm_mday << "-" << localTime.tm_year
                << " " << localTime.tm_hour << "-" << localTime.tm_min << "-" << localTime.tm_sec;
            filename = ss.str();
        }
        filename += ".csv";

        std::fstream outFile(filename);
        if (outFile.is_open()) {
            for (double sample : _samples) {
                outFile << sample << ",";
            }
            std::cout << "Timer::printToCSV() Wrote data at " << filename << std::endl;
        }
        else {
            std::cout << "Timer::printToCSV() Unable to open file at " << filename << std::endl;
        }

        outFile.close();
    }

private:
    std::chrono::high_resolution_clock::time_point _start;
    std::vector<double> _samples;
    int  _sampleWindowSize       { 100 };
    double _last                 { 0.0 };
    double _sum                  { 0.0 };
    double _avg                  { 0.0 };
    double _min                  { 0.0 };
    double _max                  { 0.0 };
};