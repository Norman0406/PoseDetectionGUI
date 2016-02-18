#include "depthcamerastream.h"
#include <QFileDialog>

DepthCameraStream::DepthCameraStream(QObject* parent)
    : DepthCamera(parent),
      m_streamReader(0)
{
}

DepthCameraStream::~DepthCameraStream()
{
    close();
}

bool DepthCameraStream::iOpen()
{
    QString fileName = QFileDialog::getOpenFileName(0, "Open File Stream", QString(), "Depth File Streams (*.seq)");
    if (fileName.isEmpty())
        return false;

    // UNDONE

    m_width = 640;
    m_height = 480;
    return true;
}

void DepthCameraStream::iClose()
{
}

void DepthCameraStream::iWaitForData()
{
}
