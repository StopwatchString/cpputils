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
// getFrameData()
//---------------------------------------------------------
StreamCopier::FrameData StreamCopier::getFrameData()
{
    std::lock_guard<std::mutex> lock(m_FrameDataLock);
    return m_FrameData;
}

//---------------------------------------------------------
// addStreamTexture()
//---------------------------------------------------------
void StreamCopier::addStreamTexture(StreamTexture* streamTexture)
{
    std::lock_guard<std::mutex> lock(m_StreamTexturesLock);
    auto iter = std::find(std::begin(m_StreamTextures), std::end(m_StreamTextures), streamTexture);
    if (iter == std::end(m_StreamTextures)) {
        m_StreamTextures.push_back(streamTexture);
    }
}

//---------------------------------------------------------
// removeStreamTexture()
//---------------------------------------------------------
void StreamCopier::removeStreamTexture(StreamTexture* streamTexture)
{
    std::lock_guard<std::mutex> lock(m_StreamTexturesLock);
    auto iter = std::find(std::begin(m_StreamTextures), std::end(m_StreamTextures), streamTexture);
    if (iter != std::end(m_StreamTextures)) {
        m_StreamTextures.erase(iter);
    }
}

//---------------------------------------------------------
// removeStreamTexture()
//---------------------------------------------------------
void StreamCopier::clearStreamTextureList()
{
    std::lock_guard<std::mutex> lock(m_StreamTexturesLock);
    m_StreamTextures.clear();
}

//---------------------------------------------------------
// updateCaptureConfig()
//---------------------------------------------------------
void StreamCopier::updateCaptureConfig(CaptureConfig inCaptureConfig)
{
    if (!m_Thread.joinable()) {
        m_CaptureConfig = inCaptureConfig;
    }
    else {
        std::cout << "WARNING StreamCopier::updateCaptureConfig() Function called while thread is running. New config ignored." << std::endl;
    }
}

//---------------------------------------------------------
// start()
//---------------------------------------------------------
void StreamCopier::start()
{
    if (!m_Thread.joinable()) {
        m_Thread = std::thread(&StreamCopier::threadFunc, this);
    }
    else {
        std::cout << "WARNING StreamCopier::start() Function called while thread is already running." << std::endl;
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

    cv::Mat cvFrame;
    if (cvVideoCapture.isOpened()) {
        // Read an initial frame to set our FrameData settings before we report running
        while (cvFrame.empty() && cvVideoCapture.isOpened()) {
            cvVideoCapture.read(cvFrame);
        }

        m_FrameData.width = cvFrame.cols;
        m_FrameData.height = cvFrame.rows;
        m_FrameData.depth = cvFrame.channels();

        m_Running = true;
    }
    else {
        std::cout << "ERROR StreamCopier::threadFunc() Could not open OpenCV Capture stream." << std::endl;
    }

    while (m_Running) {
        // Get frame
        if (cvVideoCapture.read(cvFrame)) {
            
            uint32_t size = cvFrame.cols * cvFrame.rows * cvFrame.channels();

            std::lock_guard<std::mutex> lock(m_StreamTexturesLock);
            for (StreamTexture* texture : m_StreamTextures) {
                texture->copyDataInto(cvFrame.data, size);
            }
        }
    }
}