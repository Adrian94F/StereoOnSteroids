#include "Cameras.hpp"

Cameras::Cameras()
{
    capL.open(camL);
    capR.open(camR);
    checkCameras();
}

void Cameras::checkCameras()
{
    if (!capL.isOpened())
    {
        throw "Cam 1 error";
    }
    if (!capR.isOpened())
    {
        throw "Cam 2 error";
    }
}

Mat Cameras::getLeft()
{
    Mat out;
    capL >> out;
    return out;
}

Mat Cameras::getRight()
{
    Mat out;
    capR >> out;
    return out;
}