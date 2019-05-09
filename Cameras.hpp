#ifndef CAMERAS_HPP
#define CAMERAS_HPP

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

#include "ICameras.hpp"

using namespace std;
using namespace cv;

class Cameras : public ICameras
{
public:
    Cameras();
    Mat getLeft();
    Mat getRight();
private:
    void checkCameras();
    VideoCapture capL;
    VideoCapture capR;
    int camL = 0;
    int camR = 2;
};

#endif //CAM_CAMERAS_HPP