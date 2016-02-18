#include "depthcamera.h"
#include <string.h>

DepthCamera::DepthCamera(QObject* parent)
    : QObject(parent),
      m_width(0),
      m_height(0),
      m_depthData(0),
      m_pointsData(0)
{
}

DepthCamera::~DepthCamera()
{
}

bool DepthCamera::open()
{
    if (m_depthData || m_pointsData)
        return false;

    if (!iOpen())
        return false;

    if (m_width == 0 || m_height == 0)
        return false;

    m_depthData = new float[m_width * m_height];
    m_depthDataSize = m_width * m_height;
    m_pointsData = new float[m_width* m_height * 3];
    m_pointsDataSize = m_width * m_height * 3;

    return true;
}

void DepthCamera::close()
{
    iClose();

    m_width = 0;
    m_height = 0;
    delete[] m_depthData;
    m_depthData = 0;
    delete[] m_pointsData;
    m_pointsData = 0;
}

void DepthCamera::waitForData()
{
    memset(m_depthData, 0, m_depthDataSize * sizeof(float));
    memset(m_pointsData, 0, m_pointsDataSize * sizeof(float));

    iWaitForData();
}

int DepthCamera::getWidth() const
{
    return m_width;
}

int DepthCamera::getHeight() const
{
    return m_height;
}

float* DepthCamera::getDepthData(int& depthDataSize) const
{
    depthDataSize = m_depthDataSize;
    return m_depthData;
}

float* DepthCamera::getPointsData(int& pointsDataSize) const
{
    pointsDataSize = m_pointsDataSize;
    return m_pointsData;
}
