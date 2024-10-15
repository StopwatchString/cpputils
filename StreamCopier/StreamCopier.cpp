#include "StreamCopier.h"

#include <thread>
#include <mutex>

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
    while (m_Running) {

    }
}