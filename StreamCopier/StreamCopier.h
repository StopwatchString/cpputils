#ifndef STREAM_COPIER_H
#define STREAM_COPIER_H

#include "opencv2/opencv.hpp"

//---------------------------------------------------------
// class StreamCopier
//---------------------------------------------------------
class StreamCopier
{
public:
    StreamCopier();
    ~StreamCopier();

    StreamCopier(const StreamCopier& other) = delete;
    StreamCopier(StreamCopier&& other) noexcept = delete;

    StreamCopier& operator=(const StreamCopier& other) = delete;
    StreamCopier& operator=(StreamCopier&& other) = delete;

private:

};

#endif