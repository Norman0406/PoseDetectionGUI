#ifndef DEPTHCAMERADUMMY_H
#define DEPTHCAMERADUMMY_H

#include "depthcamera.h"

class DepthCameraDummy
        : public DepthCamera
{
public:
    DepthCameraDummy();
    ~DepthCameraDummy();

protected:
    bool iOpen();
    void iClose();
    void iWaitForData();
};

#endif // DEPTHCAMERADUMMY_H
