#ifndef DEPTHCAMERADUMMY_H
#define DEPTHCAMERADUMMY_H

#include "depthcamera.h"

class DepthCameraDummy
        : public DepthCamera
{
    Q_OBJECT

public:
    DepthCameraDummy(QObject* parent = 0);
    ~DepthCameraDummy();

protected:
    bool iOpen();
    void iClose();
    void iWaitForData();
};

#endif // DEPTHCAMERADUMMY_H
