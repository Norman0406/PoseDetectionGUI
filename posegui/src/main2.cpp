#include <iostream>
#include <pose.h>

using namespace std;

void main()
{
    int width = 512;
    int height = 414;

    PoseContext* context = 0;
    if (poseInit(&context, width, height) != RESULT_SUCCESS || context == 0)
        return;

    int depthDataSize = width * height;
    float* depthData = new float[depthDataSize];

    int pointsDataSize = width * height * 3;
    float* pointsData = new float[pointsDataSize];

    // set input data and process data
    if (poseSetInput(context, depthData, depthDataSize * sizeof(float), pointsData, pointsDataSize * sizeof(float)) != RESULT_SUCCESS)
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
}
