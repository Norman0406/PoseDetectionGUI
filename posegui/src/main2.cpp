#include <iostream>
#include <pose.h>
#include <depthcamera/depthcameradimager.h>
#include <depthcamera/depthcamerakinectsdk.h>
#include <depthcamera/depthcamerakinectsdk2.h>

using namespace std;

void main()
{
    DepthCamera* camera = new DepthCameraDImager();
    if (!camera->open())
        return;

    PoseContext* context = 0;
    if (poseInit(&context, camera->getWidth(), camera->getHeight()) != RESULT_SUCCESS || context == 0)
        return;

    int depthDataSize = 0;
    float* depthData = camera->getDepthData(depthDataSize);

    int pointsDataSize = 0;
    float* pointsData = camera->getPointsData(pointsDataSize);

    // set input data and process data
    if (poseSetInput(context, depthData, depthDataSize, pointsData, pointsDataSize) != RESULT_SUCCESS)
        return;

    // get data
    PoseScene* scene = 0;
    if (poseGetScene(context, &scene) != RESULT_SUCCESS || scene == 0)
        return;

    if (poseFreeScene(scene) != RESULT_SUCCESS)
        return;

    delete[] depthData;
    delete[] pointsData;

    poseShutdown(context);

    camera->close();
    delete camera;
}
