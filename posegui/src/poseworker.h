#ifndef POSEWORKER_H
#define POSEWORKER_H

#include <QObject>
#include <QImage>
#include <QMap>

#include <pose.h>
#include <depthcamera/depthcameradummy.h>
#include <depthcamera/depthcameradimager.h>
#include <depthcamera/depthcamerakinectsdk.h>
#include <depthcamera/depthcamerakinectsdk2.h>

class PoseWorker : public QObject
{
    Q_OBJECT
public:
    explicit PoseWorker(std::function<DepthCamera*(QObject*)> factory, QObject *parent = 0);
    ~PoseWorker();

    const QImage& getImage(PoseImageType type) const;

public slots:
    bool open();
    void poseThread();
    void shutdown();

signals:
    void error(QString error);
    void hasStarted();
    void hasResults();
    void threadFinished();

private:
    void fillDepthMap(int width, int height, const float* data);

    std::function<DepthCamera*(QObject*)> m_factory;

    DepthCamera* m_camera;
    PoseContext* m_context;
    bool m_terminate;
    QMutex m_mutex;

    QMutex m_depthMapMutex;
    QImage m_depthMap;
};

#endif // POSEWORKER_H
