#ifndef CAMERATHREAD_H
#define CAMERATHREAD_H

#include <QObject>

class CameraThread : public QObject
{
    Q_OBJECT
public:
    explicit CameraThread(QObject *parent = 0);
    ~CameraThread();

signals:

public slots:
};

#endif // CAMERATHREAD_H
