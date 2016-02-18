#include "depthcameradummy.h"

DepthCameraDummy::DepthCameraDummy(QObject* parent)
    : DepthCamera(parent)
{
}

DepthCameraDummy::~DepthCameraDummy()
{
    close();
}

bool DepthCameraDummy::iOpen()
{
    m_width = 640;
    m_height = 480;
    return true;
}

void DepthCameraDummy::iClose()
{
}

void DepthCameraDummy::iWaitForData()
{
}
