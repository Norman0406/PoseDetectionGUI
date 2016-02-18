#include "poseworker.h"

PoseWorker::PoseWorker(std::function<DepthCamera*(QObject*)> factory, QObject *parent)
    : QObject(parent),
      m_factory(factory),
      m_terminate(false),
      m_camera(0),
      m_context(0)
{
}

PoseWorker::~PoseWorker()
{
}

bool PoseWorker::open()
{
    m_camera = m_factory(this);
    if (!m_camera) {
        emit error("Could not create camera from factory!");
        emit threadFinished();
        return false;
    }

    if (!m_camera->open()) {
        emit error("Could not open camera!");
        delete m_camera;
        emit threadFinished();
        return false;
    }

    if (poseInit(&m_context, m_camera->getWidth(), m_camera->getHeight()) != RESULT_SUCCESS || m_context == 0) {
        emit error("Could not init pose framework!");
        m_camera->close();
        delete m_camera;
        emit threadFinished();
        return false;
    }

    return true;
}

void PoseWorker::poseThread()
{
    emit hasStarted();

    do {
        m_mutex.lock();
        if (m_terminate) {
            m_mutex.unlock();
            break;
        }
        m_mutex.unlock();

        m_camera->waitForData();

        int depthDataSize = 0;
        float* depthData = m_camera->getDepthData(depthDataSize);

        int pointsDataSize = 0;
        float* pointsData = m_camera->getPointsData(pointsDataSize);

        // set input data and process data
        if (poseSetInput(m_context, depthData, depthDataSize, pointsData, pointsDataSize) != RESULT_SUCCESS)
            break;

        // get data
        PoseScene* scene = 0;
        if (poseGetScene(m_context, &scene) != RESULT_SUCCESS || scene == 0)
            break;

        if (poseFreeScene(scene) != RESULT_SUCCESS)
            break;

        // get depth map from api
        m_depthMapMutex.lock();
        int imageWidth, imageHeight, imageSize;
        float* data = 0;
        poseGetImage(m_context, IMAGE_DEPTH, &imageWidth, &imageHeight, &imageSize, (void**)&data);
        fillDepthMap(imageWidth, imageHeight, data);
        m_depthMapMutex.unlock();

        emit hasResults();
    } while (true);

    poseShutdown(m_context);

    m_camera->close();
    delete m_camera;

    emit threadFinished();
}

void PoseWorker::fillDepthMap(int width, int height, const float* data)
{
    if (m_depthMap.height() != height || m_depthMap.width() != width)
        m_depthMap = QImage(width, height, QImage::Format_RGB32);

    for (int y = 0; y < height; y++) {
        QRgb* imageRow = (QRgb*)m_depthMap.scanLine(y);
        const float* dataRow = data + (y * width);
        for (int x = 0; x < width; x++) {
            uchar val = dataRow[x] * 20;

            imageRow[x] = qRgba(val, val, val, 255);
        }
    }
}

const QImage& PoseWorker::getImage(PoseImageType type) const
{
    switch (type) {
    case IMAGE_DEPTH:
        return m_depthMap;
    }

    // UNDONE
    return QImage();
}

void PoseWorker::shutdown()
{
    QMutexLocker locker(&m_mutex);
    m_terminate = true;
}
