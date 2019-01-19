#ifndef CAMERAS_HPP
#define CAMERAS_HPP

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

class Cameras {
public:
    Cameras();
    Mat getLeft();
    Mat getRight();
    Mat getBoth();
    static Mat resizeAndConcat(Mat imL, Mat imR, int d=2);
private:
    void checkCameras();
    VideoCapture capL;
    VideoCapture capR;
    int camL = 4;
    int camR = 2;
};

#endif //CAM_CAMERAS_HPP