#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QVariant>

#include <functional>

namespace Ui {
    class MainWindow;
}

class PoseWorker;
class DepthCamera;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_connectButton_clicked();
    void on_disconnectButton_clicked();
    void handlePoseResults();
    void poseStarted();
    void poseFinished();
    void poseError(QString message);

    void on_displaySelector_currentIndexChanged(int index);

signals:

private:
    Ui::MainWindow* ui;
    QThread m_poseThread;
    PoseWorker* m_poseWorker;
    QVariant m_imageType;

    std::map<QString, std::function<DepthCamera*(QObject*)>> m_cameraFactory;
};

#endif // MAINWINDOW_H
