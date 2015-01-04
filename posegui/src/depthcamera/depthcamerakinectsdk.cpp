#include "depthcamerakinectsdk.h"
#include <string.h>

DepthCameraKinectSDK::DepthCameraKinectSDK()
    : m_sensor(0),
      m_processThread(0),
      m_terminate(false),
      m_depthBuffer(0),
      m_pointsBuffer(0)
{
}

DepthCameraKinectSDK::~DepthCameraKinectSDK()
{
    close();
}

bool DepthCameraKinectSDK::iOpen()
{
    NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH);

    if (FAILED(NuiCreateSensorByIndex(0, &m_sensor)))
        return false;

    m_nextDepthFrameEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

    // Open a depth image stream to receive depth frames
    if (FAILED(m_sensor->NuiImageStreamOpen(NUI_IMAGE_TYPE_DEPTH, NUI_IMAGE_RESOLUTION_640x480,
                                            0, 2, m_nextDepthFrameEvent, &m_depthStreamHandle))) {
        return false;
    }

    DWORD width = 0, height = 0;
    NuiImageResolutionToSize(NUI_IMAGE_RESOLUTION_640x480, width, height);

    m_pointsBuffer = new float[height * width * 3];
    m_depthBuffer = new float[height * width];
    m_depthMapReady = false;

    m_width = width;
    m_height = height;

    m_terminate = false;
    m_processThread = new std::thread(std::bind(&DepthCameraKinectSDK::processDepth, this));

    return true;
}

void DepthCameraKinectSDK::iClose()
{
    if (m_processThread) {
        m_terminate = true;
        m_processThread->join();
        delete m_processThread;
    }

    m_sensor->Release();

    delete[] m_depthBuffer;
    delete[] m_pointsBuffer;
}

void DepthCameraKinectSDK::processDepth()
{
    while (!m_terminate) {
        if (WaitForSingleObject(m_nextDepthFrameEvent, INFINITE) == WAIT_OBJECT_0) {
            // process depth

            NUI_IMAGE_FRAME frame;
            m_sensor->NuiImageStreamGetNextFrame(m_depthStreamHandle, 0, &frame);

            INuiFrameTexture* texture;
            m_sensor->NuiImageFrameGetDepthImagePixelFrameTexture(m_depthStreamHandle, &frame, nullptr, &texture);

            NUI_LOCKED_RECT rect;
            texture->LockRect(0, &rect, nullptr, 0);

            NUI_DEPTH_IMAGE_PIXEL* bufferRun = reinterpret_cast<NUI_DEPTH_IMAGE_PIXEL*>(rect.pBits);

            std::unique_lock<std::mutex> lock(m_depthMapMutex);

            memset(m_depthBuffer, 0, m_width * m_height * sizeof(float));
            memset(m_pointsBuffer, 0, m_width * m_height * 3 * sizeof(float));

            float* bufferRunDepthData = m_depthBuffer;
            float* bufferRunPointsData = m_pointsBuffer;
            for (int i = 0; i < m_height; i++) {
                for (int j = 0; j < m_width; j++) {
                    NUI_DEPTH_IMAGE_PIXEL depthPixel = *bufferRun++;
                    *bufferRunDepthData = (float)(depthPixel.depth / 1000.0f);

                    // TODO: get point cloud

                    bufferRunDepthData++;
                    bufferRunPointsData+=3;
                }
            }

            m_depthMapReady = true;
            m_depthMapReadyCond.notify_all();
            lock.unlock();

            texture->UnlockRect(0);
            m_sensor->NuiImageStreamReleaseFrame(m_depthStreamHandle, &frame);
        }
    }
}

void DepthCameraKinectSDK::iWaitForData()
{
    std::unique_lock<std::mutex> lock(m_depthMapMutex);
    while (!m_depthMapReady)
        m_depthMapReadyCond.wait(lock);

    memcpy(m_depthData, m_depthBuffer, m_depthDataSize * sizeof(float));
    memcpy(m_pointsData, m_pointsBuffer, m_pointsDataSize * sizeof(float));

    m_depthMapReady = false;
}
