#ifndef STREAM_COPIER_H
#define STREAM_COPIER_H

#include "opencv2/opencv.hpp"

//---------------------------------------------------------
// class StreamCopier
//---------------------------------------------------------
class StreamCopier
{
public:
    enum class CAPTURE_TYPE {
        INVALID_TYPE,
        STREAM,
        DIRECTSHOW
    };

    struct CaptureConfig {
        CAPTURE_TYPE captureType{ CAPTURE_TYPE::INVALID_TYPE };
        std::string streamAddress;
        std::string directshowDeviceName;
        int directshowDeviceIndex{ 0 };
    };

    StreamCopier();
    StreamCopier(CaptureConfig inCaptureConfig);
    ~StreamCopier();

    void updateCaptureConfig(CaptureConfig inCaptureConfig);
    void start();
    void stop();
    bool isRunning() { return m_Running; };

    StreamCopier(const StreamCopier& other) = delete;
    StreamCopier(StreamCopier&& other) noexcept = delete;

    StreamCopier& operator=(const StreamCopier& other) = delete;
    StreamCopier& operator=(StreamCopier&& other) = delete;

private:
    CaptureConfig m_CaptureConfig;
    bool m_Running;
    std::thread m_Thread;
    void threadFunc();
};

#endif