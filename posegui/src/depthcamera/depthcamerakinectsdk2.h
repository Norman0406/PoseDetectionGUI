#ifndef DEPTHCAMERAKINECTSDK2_H
#define DEPTHCAMERAKINECTSDK2_H

#include "depthcamera.h"
#include <Kinect.h>
#include <Windows.h>
#include <thread>
#include <mutex>
#include <condition_variable>

class DepthCameraKinectSDK2
        : public DepthCamera
{
    Q_OBJECT

public:
    DepthCameraKinectSDK2(QObject* parent = 0);
    ~DepthCameraKinectSDK2();

    float getMinReliableDistance() const;
    float getMaxReliableDistance() const;
    float getFOVHorizontal() const;
    float getFOVVertical() const;
    float getFOVDiagonal() const;

protected:    
    bool iOpen();
    void iClose();
    void iWaitForData();

private:
    void processDepth();

    IKinectSensor* m_sensor;
    IDepthFrameSource* m_depthFrameSource;
    IDepthFrameReader* m_depthFrameReader;
    HANDLE m_depthEventHandle;
    ICoordinateMapper* m_mapper;
    CameraSpacePoint* m_points;

    float m_minReliableDistance;
    float m_maxReliableDistance;
    float m_fovHorizontal;
    float m_fovVertical;
    float m_fovDiagonal;

    std::thread* m_processThread;
    std::mutex m_terminateMutex;
    bool m_terminate;

    std::mutex m_depthMapMutex;
    std::condition_variable m_depthMapReadyCond;
    bool m_depthMapReady;
    float* m_depthBuffer;
    float* m_pointsBuffer;
};

#endif // DEPTHCAMERAKINECTSDK2_H
