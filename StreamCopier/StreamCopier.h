#ifndef STREAM_COPIER_H
#define STREAM_COPIER_H

#include "StreamTexture.h"

#include "opencv2/opencv.hpp"

#include <vector>
#include <thread>
#include <mutex>

//---------------------------------------------------------
// class StreamCopier
//---------------------------------------------------------
class StreamCopier
{
public:
    struct CaptureConfig {
        cv::VideoCaptureAPIs api;
        std::string streamAddress;
        std::string directshowDeviceName;
        int directshowDeviceIndex{ 0 };
    };

    struct FrameData {
        int width = 0;
        int height = 0;
        int depth = 0;
    };

    StreamCopier();
    StreamCopier(CaptureConfig inCaptureConfig);
    ~StreamCopier();

    FrameData getFrameData();

    void addStreamTexture(StreamTexture* streamTexture);
    void removeStreamTexture(StreamTexture* streamTexture);
    void clearStreamTextureList();

    void updateCaptureConfig(CaptureConfig inCaptureConfig);
    void start();
    void stop();
    bool isRunning() { return m_Running; };

    StreamCopier(const StreamCopier& other) = delete;
    StreamCopier(StreamCopier&& other) noexcept = delete;

    StreamCopier& operator=(const StreamCopier& other) = delete;
    StreamCopier& operator=(StreamCopier&& other) = delete;

private:
    void threadFunc();

    FrameData m_FrameData;
    std::mutex m_FrameDataLock;
    std::vector<StreamTexture*> m_StreamTextures;
    std::mutex m_StreamTexturesLock;
    CaptureConfig m_CaptureConfig;
    bool m_Running;
    std::thread m_Thread;
};

#endif