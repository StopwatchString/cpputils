#include "StreamCopier.h"
#include "StreamTexture.h"

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

    return EXIT_SUCCESS;
}