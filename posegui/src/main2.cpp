#include <iostream>
#include <pose.h>

using namespace std;

void main()
{
    IPoseContext* context = 0;
    if (PoseInit(&context) || context == 0)
        return;



    PoseShutdown(context);
}
