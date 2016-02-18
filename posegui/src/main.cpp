#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    MainWindow window;
    window.show();
    return application.exec();
}

/*#include <iostream>
#include <pose.h>
#include <depthcamera/depthcameradummy.h>
#include <depthcamera/depthcameradimager.h>
#include <depthcamera/depthcamerakinectsdk.h>
#include <depthcamera/depthcamerakinectsdk2.h>

using namespace std;

void main()
{


    DepthCamera* camera = new DepthCameraDummy();
    //DepthCamera* camera = new DepthCameraDImager();
    //DepthCamera* camera = new DepthCameraKinectSDK2();
    if (!camera->open())
        return;

    PoseContext* context = 0;
    if (poseInit(&context, camera->getWidth(), camera->getHeight()) != RESULT_SUCCESS || context == 0)
        return;

    do {
        camera->waitForData();

        int depthDataSize = 0;
        float* depthData = camera->getDepthData(depthDataSize);

        int pointsDataSize = 0;
        float* pointsData = camera->getPointsData(pointsDataSize);

        // set input data and process data
        if (poseSetInput(context, depthData, depthDataSize, pointsData, pointsDataSize) != RESULT_SUCCESS)
            break;

        // get data
        PoseScene* scene = 0;
        if (poseGetScene(context, &scene) != RESULT_SUCCESS || scene == 0)
            break;

        if (poseFreeScene(scene) != RESULT_SUCCESS)
            break;
    } while (true);

    poseShutdown(context);

    camera->close();
    delete camera;
}*/
