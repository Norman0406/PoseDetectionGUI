#ifndef DEPTHCAMERAKINECTSDK_H
#define DEPTHCAMERAKINECTSDK_H

#include "depthcamera.h"
#include <Windows.h>
#include <NuiApi.h>
#include <thread>
#include <mutex>
#include <condition_variable>

class DepthCameraKinectSDK
        : public DepthCamera
{
    Q_OBJECT

public:
    DepthCameraKinectSDK(QObject* parent = 0);
    ~DepthCameraKinectSDK();

protected:
    bool iOpen();
    void iClose();
    void iWaitForData();

private:
    void processDepth();

    INuiSensor* m_sensor;
    HANDLE m_nextDepthFrameEvent;
    HANDLE m_depthStreamHandle;

    std::thread* m_processThread;
    std::mutex m_terminateMutex;
    bool m_terminate;

    std::mutex m_depthMapMutex;
    std::condition_variable m_depthMapReadyCond;
    bool m_depthMapReady;
    float* m_depthBuffer;
    float* m_pointsBuffer;
};

#endif // DEPTHCAMERAKINECTSDK_H
