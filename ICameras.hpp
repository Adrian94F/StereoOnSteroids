#ifndef ICAMERAS_HPP
#define ICAMERAS_HPP

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>

using namespace std;
using namespace cv;

class ICameras {
public:
    virtual Mat getLeft() = 0;
    virtual Mat getRight() = 0;
    Mat getBoth();
    static Mat resizeAndConcat(Mat imL, Mat imR, int d=2);
};

#endif //CAMERAS_HPP