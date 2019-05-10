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
    int lCamNumber_ = 0;
    int rCamNumber_ = 2;
};

#endif //CAM_CAMERAS_HPP