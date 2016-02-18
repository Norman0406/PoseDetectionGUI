#ifndef DEPTHCAMERASTREAM_H
#define DEPTHCAMERASTREAM_H

#include "depthcamera.h"

namespace pose {
class StreamReader;
}

class DepthCameraStream
        : public DepthCamera
{
    Q_OBJECT

public:
    DepthCameraStream(QObject* parent = 0);
    ~DepthCameraStream();

protected:
    bool iOpen();
    void iClose();
    void iWaitForData();

private:
    pose::StreamReader* m_streamReader;
};

#endif // DEPTHCAMERASTREAM_H
