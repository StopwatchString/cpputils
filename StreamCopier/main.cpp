#include "StreamCopier.h"
#include "StreamTexture.h"

#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>

int main()
{
    StreamCopier::CaptureConfig captureConfig;
    captureConfig.api = cv::CAP_DSHOW;
    captureConfig.directshowDeviceIndex = 0;

    StreamCopier streamCopier(captureConfig);
    streamCopier.start();

    std::vector<StreamTexture> streamTextures;

    if (streamCopier.isRunning()) {
        StreamCopier::FrameData frameData = streamCopier.getFrameData();

        streamTextures.emplace_back("name", frameData.width, frameData.height, frameData.depth);

        auto it = std::find_if(std::begin(streamTextures), std::end(streamTextures), [](const StreamTexture& streamTexture) { return streamTexture.getName() == "name"; });
        if (it != std::end(streamTextures)) {
            streamCopier.addStreamTexture(&*it);
        }
    }

    std::string s;
    std::getline(std::cin, s);

    return EXIT_SUCCESS;
}