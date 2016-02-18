#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "poseworker.h"

#include <depthcamera/depthcameradummy.h>
#include <depthcamera/depthcamerastream.h>
#include <depthcamera/depthcameradimager.h>
#include <depthcamera/depthcamerakinectsdk.h>
#include <depthcamera/depthcamerakinectsdk2.h>

#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_poseWorker(0)
{
    ui->setupUi(this);

    ui->displaySelector->addItem("Depth Map", QVariant(IMAGE_DEPTH));
    ui->displaySelector->addItem("Background", QVariant(IMAGE_USERSEGMENTATION));
    ui->displaySelector->addItem("Foreground", QVariant(IMAGE_USERSEGMENTATION));
    ui->displaySelector->addItem("Regions", QVariant(IMAGE_USERSEGMENTATION));
    ui->displaySelector->addItem("User Segmentation", QVariant(IMAGE_USERSEGMENTATION));

    m_cameraFactory["Dummy"] = [](QObject* parent) { return new DepthCameraDummy(parent); };
    m_cameraFactory["File Stream"] = [](QObject* parent) { return new DepthCameraStream(parent); };
    m_cameraFactory["Panasonic D-Imager"] = [](QObject* parent) { return new DepthCameraDImager(parent); };
    m_cameraFactory["Microsoft Kinect"] = [](QObject* parent) { return new DepthCameraKinectSDK(parent); };
    m_cameraFactory["Microsoft Kinect v2"] = [](QObject* parent) { return new DepthCameraKinectSDK2(parent); };

    for (auto it : m_cameraFactory) {
        ui->cameraSelector->addItem(it.first);
    }

    m_imageType = QVariant(IMAGE_DEPTH);
}

MainWindow::~MainWindow()
{
    if (m_poseWorker) {
        m_poseWorker->shutdown();
        m_poseThread.wait();
        m_poseThread.quit();
    }

    delete ui;
}

void MainWindow::on_connectButton_clicked()
{
    ui->connectButton->setEnabled(false);
    ui->cameraSelector->setEnabled(false);

    const std::function<DepthCamera*(QObject*)>& factory = m_cameraFactory[ui->cameraSelector->currentText()];

    m_poseWorker = new PoseWorker(factory);
    connect(m_poseWorker, &PoseWorker::hasResults, this, &MainWindow::handlePoseResults);
    connect(m_poseWorker, &PoseWorker::hasStarted, this, &MainWindow::poseStarted);
    connect(m_poseWorker, &PoseWorker::threadFinished, this, &MainWindow::poseFinished);
    connect(m_poseWorker, &PoseWorker::error, this, &MainWindow::poseError);

    if (m_poseWorker->open()) {
        m_poseWorker->moveToThread(&m_poseThread);
        connect(&m_poseThread, &QThread::finished, m_poseWorker, &PoseWorker::deleteLater);
        connect(&m_poseThread, &QThread::started, m_poseWorker, &PoseWorker::poseThread);

        // start thread
        m_poseThread.start();
    }
}

void MainWindow::handlePoseResults()
{
    ui->imageViewer->setData(m_poseWorker->getImage((PoseImageType)m_imageType.toInt()));
}

void MainWindow::on_disconnectButton_clicked()
{
    ui->disconnectButton->setEnabled(false);
    ui->displaySelector->setEnabled(false);

    m_poseWorker->shutdown();
    m_poseThread.quit();
}

void MainWindow::on_displaySelector_currentIndexChanged(int index)
{
    m_imageType = ui->displaySelector->itemData(index);
}

void MainWindow::poseStarted()
{
    ui->disconnectButton->setEnabled(true);
    ui->displaySelector->setEnabled(true);
}

void MainWindow::poseFinished()
{
    m_poseThread.wait();

    // no need to call delete, since deleteLater will be called from thread
    m_poseWorker = 0;

    ui->connectButton->setEnabled(true);
    ui->cameraSelector->setEnabled(true);
}

void MainWindow::poseError(QString message)
{
    QMessageBox::warning(this, "Pose SDK", message, QMessageBox::Ok);
}
