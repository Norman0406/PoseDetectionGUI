#include "depthcamerakinectsdk2.h"

DepthCameraKinectSDK2::DepthCameraKinectSDK2()
    : m_sensor(0),
      m_depthFrameSource(0),
      m_depthFrameReader(0),
      m_depthBuffer(0),
      m_pointsBuffer(0)
{
}

DepthCameraKinectSDK2::~DepthCameraKinectSDK2()
{
    close();
}

bool DepthCameraKinectSDK2::open()
{
    if (FAILED(GetDefaultKinectSensor(&m_sensor)))
        return false;

    if (FAILED(m_sensor->Open()))
        return false;

    if (FAILED(m_sensor->get_DepthFrameSource(&m_depthFrameSource)))
        return false;

    if (FAILED(m_depthFrameSource->OpenReader(&m_depthFrameReader)))
        return false;

    UINT16 minReliableDistance, maxReliableDistance;
    m_depthFrameSource->get_DepthMinReliableDistance(&minReliableDistance);
    m_depthFrameSource->get_DepthMaxReliableDistance(&maxReliableDistance);
    m_minReliableDistance = minReliableDistance / 1000.0f;
    m_maxReliableDistance = maxReliableDistance / 1000.0f;

    IFrameDescription* frameDesc;
    if (FAILED(m_depthFrameSource->get_FrameDescription(&frameDesc)))
        return false;

    frameDesc->get_HorizontalFieldOfView(&m_fovHorizontal);
    frameDesc->get_VerticalFieldOfView(&m_fovVertical);
    frameDesc->get_DiagonalFieldOfView(&m_fovDiagonal);

    unsigned int bytesPerPixel;
    frameDesc->get_BytesPerPixel(&bytesPerPixel);

    frameDesc->get_Width(&m_width);
    frameDesc->get_Height(&m_height);

    m_pointsBuffer = new float[m_height * m_width * 3];
    m_depthBuffer = new float[m_height * m_width];
    m_depthMapReady = false;

    m_points = new CameraSpacePoint[m_width * m_height];

    m_depthFrameReader->SubscribeFrameArrived((WAITABLE_HANDLE*)&m_depthEventHandle);

    m_sensor->get_CoordinateMapper(&m_mapper);

    m_terminate = false;
    m_processThread = new std::thread(std::bind(&DepthCameraKinectSDK2::processDepth, this));

    return true;
}

void DepthCameraKinectSDK2::close()
{
    if (m_processThread) {
        m_terminate = true;
        m_processThread->join();
        delete m_processThread;
    }

    m_sensor->Close();
    m_sensor->Release();
    delete[] m_points;
    delete[] m_depthBuffer;
    delete[] m_pointsBuffer;
}

void DepthCameraKinectSDK2::processDepth()
{
    while (!m_terminate) {
        if (WaitForSingleObject(m_depthEventHandle, INFINITE) == WAIT_OBJECT_0) {
            IDepthFrameArrivedEventArgs* eventArgs;
            if (SUCCEEDED(m_depthFrameReader->GetFrameArrivedEventData((WAITABLE_HANDLE)m_depthEventHandle, &eventArgs) && eventArgs)) {
                IDepthFrameReference* ref;
                if (SUCCEEDED(eventArgs->get_FrameReference(&ref)) && ref) {
                    IDepthFrame* frame;
                    if (SUCCEEDED(ref->AcquireFrame(&frame)) && frame) {
                        UINT capacity;
                        UINT16* buffer;
                        frame->AccessUnderlyingBuffer(&capacity, &buffer);

                        m_mapper->MapDepthFrameToCameraSpace(capacity, buffer, capacity, m_points);

                        CameraSpacePoint* pointsBufferRun = m_points;
                        UINT16* bufferRun = buffer;

                        std::unique_lock<std::mutex> lock(m_depthMapMutex);

                        memset(m_depthBuffer, 0, m_depthDataSize);
                        memset(m_pointsBuffer, 0, m_pointsDataSize);

                        float* bufferRunDepthData = m_depthData;
                        float* bufferRunPointsData = m_pointsData;
                        for (int i = 0; i < m_height; i++) {
                            for (int j = 0; j < m_width; j++) {
                                // set depth value
                                float depth = *bufferRun / 1000.0f;
                                *bufferRunDepthData = depth;

                                // set point in point cloud
                                bufferRunPointsData[0] = (*pointsBufferRun).X;
                                bufferRunPointsData[1] = (*pointsBufferRun).Y;
                                bufferRunPointsData[2] = (*pointsBufferRun).Z;

                                bufferRun++;
                                pointsBufferRun++;
                                bufferRunDepthData++;
                                bufferRunPointsData+=3;
                            }
                        }

                        m_depthMapReady = true;
                        m_depthMapReadyCond.notify_all();
                        lock.unlock();

                        frame->Release();
                    }
                }
            }
        }
    }
}

float DepthCameraKinectSDK2::getMinReliableDistance() const
{
    return m_minReliableDistance;
}

float DepthCameraKinectSDK2::getMaxReliableDistance() const
{
    return m_maxReliableDistance;
}

float DepthCameraKinectSDK2::getFOVHorizontal() const
{
    return m_fovHorizontal;
}

float DepthCameraKinectSDK2::getFOVVertical() const
{
    return m_fovVertical;
}

float DepthCameraKinectSDK2::getFOVDiagonal() const
{
    return m_fovDiagonal;
}

void DepthCameraKinectSDK2::iWaitForData()
{
    std::unique_lock<std::mutex> lock(m_depthMapMutex);
    while (!m_depthMapReady)
        m_depthMapReadyCond.wait(lock);

    memcpy(m_depthBuffer, m_depthData, m_depthDataSize);
    memcpy(m_pointsBuffer, m_pointsData, m_pointsDataSize);

    m_depthMapReady = false;
}
