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
    Mat getLeft() override;
    Mat getRight() override;
private:
    void checkCameras();
    VideoCapture lCapture_;
    VideoCapture rCapture_;
    int lCamNumber_ = 2;
    int rCamNumber_ = 0;
};

#endif //CAM_CAMERAS_HPP