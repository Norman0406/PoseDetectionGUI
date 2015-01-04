#include "depthcameradimager.h"
#include "Dimagerdll.h"
#include <math.h>

const float DepthCameraDImager::m_sx    = 0.000024f;
const float DepthCameraDImager::m_sy    = 0.000024f;
const int DepthCameraDImager::m_uc      = 80;
const int DepthCameraDImager::m_vc      = 60;
const int DepthCameraDImager::m_k       = -36120;
const float DepthCameraDImager::m_f     = 0.00396f;

DepthCameraDImager::DepthCameraDImager()
    : m_kdat(0), m_ndat(0)
{
    m_width = 160;
    m_height = 120;
    m_isInit = false;
}

DepthCameraDImager::~DepthCameraDImager()
{
    close();
}

bool DepthCameraDImager::iOpen()
{
    int ret = InitImageDriver();
    if (ret != 0)
        return false;
    m_isInit = true;

    // go to non-sleep mode
    if (!setSleep(1))
        return false;

    // set framerate
    if (!setFramerate(30))
        return false;

    // set frequency
    if (!setFrequency(0))
        return false;

    // set a default grayscale threshold to filter out noise
    setThreshold(5);

    m_kdat = new unsigned short[m_width * m_height];
    m_ndat = new unsigned short[m_width * m_height];

    return true;
}

void DepthCameraDImager::iClose()
{
    if (m_isInit) {
        FreeImageDriver();
        m_isInit = false;

        delete[] m_kdat;
        m_kdat = 0;
        delete[] m_ndat;
        m_ndat = 0;
    }
}

bool DepthCameraDImager::setFrequency(int frq)
{
    if (!m_isInit)
        return false;

    int ret = ChangeFreq(frq);
    if (ret != 0)
        return false;

    if (Freqmode() != frq) {
        ret = ChangeFreq(frq);
        if (ret != 0 || Freqmode() != frq)
            return false;
    }

    return true;
}

bool DepthCameraDImager::getFrequency(int& frq) const
{
    if (!m_isInit)
        return false;

    int ret = Freqmode();
    if (ret == 4)
        return false;

    frq = ret;
    return true;
}

bool DepthCameraDImager::setFramerate(int framerate)
{
    if (!m_isInit)
        return false;

    int ret = ChangeSpeed(framerate);
    if (ret != 0)
        return false;

    if (Speedmode() != framerate) {
        ret = ChangeSpeed(framerate);
        if (ret != 0 || Speedmode() != framerate)
            return false;
    }

    return true;
}

bool DepthCameraDImager::getFramerate(int& speed) const
{
    if (!m_isInit)
        return false;

    int ret = Speedmode();
    if (ret == 99)
        return false;

    speed = ret;
    return true;
}

bool DepthCameraDImager::setSleep(int sleepMode)
{
    if (!m_isInit)
        return false;

    int ret = ChangeSleep(sleepMode);
    if (ret != 0)
        return false;

    if (Sleepmode() != sleepMode) {
        ret = ChangeSleep(sleepMode);
        if (ret != 0 || Sleepmode() != sleepMode)
            return false;
    }

    return true;
}

bool DepthCameraDImager::getSleep(int& sleepMode) const
{
    if (!m_isInit)
        return false;

    int ret = Sleepmode();
    if (ret == 4)
        return false;

    sleepMode = ret;
    return true;
}

void DepthCameraDImager::setThreshold(int threshold)
{
    if (threshold < 0)
        m_grayThreshold = 0;
    else if (threshold > 255)
        m_grayThreshold = 255;
    else
        m_grayThreshold = (unsigned short)threshold;
}

int DepthCameraDImager::getThreshold() const
{
    return m_grayThreshold;
}

void DepthCameraDImager::depthTo3D(int u, int v, float D, float& x, float& y, float& z)
{
    float tempU = (m_sx * m_sx) * ((u - m_uc) * (u - m_uc));
    float tempV = (m_sy * m_sy) * ((v - m_vc) * (v - m_vc));

    float xd = (m_sx * (u - m_uc)) / (1 + m_k * (tempU + tempV));
    float yd = (m_sy * (v - m_vc)) / (1 + m_k * (tempU + tempV));
    float d = sqrt(xd * xd + yd * yd + m_f * m_f);

    float Dd = D / d;
    x = xd * Dd;
    y = yd * Dd;
    z = m_f * Dd;
}

void DepthCameraDImager::iWaitForData()
{
    if (GetImageKN(m_kdat, m_ndat) == 0) {
        const unsigned short* bufferRunDepth = m_kdat;
        const unsigned short* bufferRunGray = m_ndat;
        float* bufferRunDepthData = m_depthData;
        float* bufferRunPointsData = m_pointsData;

        for (int i = 0; i < m_height; i++) {
            for (int j = 0; j < m_width; j++) {

                unsigned short gray = *bufferRunGray;

                if (gray > 5) {
                    // set depth value
                    float depth = *bufferRunDepth / 100.0f;
                    *bufferRunDepthData = depth;

                    // set point in point cloud
                    depthTo3D(j, i, depth, bufferRunPointsData[0], bufferRunPointsData[1], bufferRunPointsData[2]);
                }

                bufferRunDepth++;
                bufferRunGray++;
                bufferRunDepthData++;
                bufferRunPointsData+=3;
            }
        }
    }
}
