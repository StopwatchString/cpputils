#include "StreamCopier.h"

#include <thread>
#include <mutex>

//---------------------------------------------------------
// Constructor
//---------------------------------------------------------
StreamCopier::StreamCopier()
    : running(true)
{
    thread = std::thread(&StreamCopier::threadFunc, this);
}

//---------------------------------------------------------
// Destructor
//---------------------------------------------------------
StreamCopier::~StreamCopier()
{
    stop();
}

//---------------------------------------------------------
// updateConfig()
//---------------------------------------------------------
void StreamCopier::updateConfig()
{
    if (!running) {

    }
    else {
        std::cout << "WARNING StreamCopier::updateConfig() Function called while thread is running. New config ignored." << std::endl;
    }
}

//---------------------------------------------------------
// start()
//---------------------------------------------------------
void StreamCopier::start()
{
    if (!running) {
        running = true;
        thread = std::thread(&StreamCopier::threadFunc, this);
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
    if (running) {
        running = false;
        if (thread.joinable()) {
            thread.join();
        }
    }
    else {
        std::cout << "WARNING StreamCopier::stop() Function called while thread isn't running." << std::endl;
    }
}

//---------------------------------------------------------
// threadFunc()
//---------------------------------------------------------
void StreamCopier::threadFunc()
{
    while (running) {

    }
}