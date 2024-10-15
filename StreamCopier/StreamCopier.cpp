#include "StreamCopier.h"

//---------------------------------------------------------
// Constructor
//---------------------------------------------------------
StreamCopier::StreamCopier()
    : m_Running(false),
      m_CaptureConfig(),
      m_Thread()
{

}

//---------------------------------------------------------
// Config Constructor
//---------------------------------------------------------
StreamCopier::StreamCopier(CaptureConfig inCaptureConfig)
    : m_Running(false),
      m_CaptureConfig(inCaptureConfig),
      m_Thread()
{

}

//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
StreamCopier::~StreamCopier()
{
    stop();
}

//---------------------------------------------------------
// updateCaptureConfig()
//---------------------------------------------------------
void StreamCopier::updateCaptureConfig(CaptureConfig inCaptureConfig)
{
    if (!m_Running) {
        m_CaptureConfig = inCaptureConfig;
    }
    else {
        std::cout << "WARNING StreamCopier::updateCaptureConfig() Function called while thread is m_Running. New config ignored." << std::endl;
    }
}

//---------------------------------------------------------
// start()
//---------------------------------------------------------
void StreamCopier::start()
{
    if (!m_Running) {
        m_Running = true;
        m_Thread = std::thread(&StreamCopier::threadFunc, this);
    }
    else {
        std::cout << "WARNING StreamCopier::start() Function called while thread is already m_Running." << std::endl;
    }
}

//---------------------------------------------------------
// stop()
//---------------------------------------------------------
void StreamCopier::stop()
{
    if (m_Running) {
        m_Running = false;
        if (m_Thread.joinable()) {
            m_Thread.join();
        }
    }
    else {
        std::cout << "WARNING StreamCopier::stop() Function called while thread isn't m_Running." << std::endl;
    }
}

//---------------------------------------------------------
// threadFunc()
//---------------------------------------------------------
void StreamCopier::threadFunc()
{
    // Open Capture
    cv::VideoCapture cvVideoCapture;

    switch (m_CaptureConfig.api)
    {
        case cv::CAP_DSHOW:
        {
            // look up index based on config
            int index = m_CaptureConfig.directshowDeviceIndex;
            cvVideoCapture.open(index, cv::CAP_DSHOW);
            break;
        }
        case cv::CAP_FFMPEG:
        {
            cvVideoCapture.open(m_CaptureConfig.streamAddress, cv::CAP_FFMPEG);
            break;
        }
        default:
            break;
    }

    int width = 0;
    int height = 0;
    int depth = 0;

    if (!cvVideoCapture.isOpened()) {
        std::cout << "ERROR StreamCopier::threadFunc() Could not open OpenCV Capture stream." << std::endl;
        m_Running = false;
    }

    cv::Mat cvFrame;
    while (m_Running) {
        // Get frame
        if (cvVideoCapture.read(cvFrame)) {
            int width = cvFrame.cols;
            int height = cvFrame.rows;
            int depth = cvFrame.channels();
            uint32_t size = width * height * depth;
            cvFrame.data;

            for (StreamTexture* texture : m_StreamTextures) {
                void* target = texture->getTargetAddress(width, height, depth);
                if (target != nullptr) {
                    memcpy(target, cvFrame.data, size);
                }
            }
        }
    }
}