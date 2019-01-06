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

Mat Cameras::resizeAndConcat(Mat imL, Mat imR, int d)
{
    resize(imL, imL, Size(imL.cols / d, imL.rows / d));
    resize(imR, imR, Size(imR.cols / d, imR.rows / d));
    Mat result = imL;
    hconcat(result, imR, result);
    return result;
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

Mat Cameras::getBoth()
{
    return resizeAndConcat(getLeft(), getRight());
}