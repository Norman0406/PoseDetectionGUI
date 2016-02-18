#ifndef DEPTHCAMERA_H
#define DEPTHCAMERA_H

#include <QObject>

class DepthCamera
        : public QObject
{
    Q_OBJECT

public:
    virtual ~DepthCamera();

    bool open();
    void close();

    void waitForData();

    int getWidth() const;
    int getHeight() const;
    float* getDepthData(int& depthDataSize) const;
    float* getPointsData(int& pointsDataSize) const;

protected:
    explicit DepthCamera(QObject* parent = 0);

    virtual bool iOpen() = 0;
    virtual void iClose() = 0;
    virtual void iWaitForData() = 0;

    int m_width;
    int m_height;
    int m_depthDataSize;
    float* m_depthData;
    int m_pointsDataSize;
    float* m_pointsData;
};

#endif // DEPTHCAMERA_H
