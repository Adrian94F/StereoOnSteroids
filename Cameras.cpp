#include "Cameras.hpp"

Cameras::Cameras()
{
    lCapture_.open(lCamNumber_);
    rCapture_.open(rCamNumber_);
    checkCameras();
}

void Cameras::checkCameras()
{
    if (!lCapture_.isOpened())
    {
        throw "Cam 1 error";
    }
    if (!rCapture_.isOpened())
    {
        throw "Cam 2 error";
    }
}

Mat Cameras::getLeft()
{
    Mat out;
    lCapture_ >> out;
    return out;
}

Mat Cameras::getRight()
{
    Mat out;
    rCapture_ >> out;
    return out;
}