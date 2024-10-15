#include "StreamCopier.h"

#include <iostream>
#include <vector>
#include <string>

int main()
{
    StreamCopier::CaptureConfig captureConfig;
    captureConfig.api = cv::CAP_DSHOW;
    captureConfig.directshowDeviceIndex = 0;

    StreamCopier streamCopier(captureConfig);
    streamCopier.start();

    std::string temp;
    std::getline(std::cin, temp);

    return EXIT_SUCCESS;
}