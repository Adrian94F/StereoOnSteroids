#ifndef FAKECAMERAS_HPP
#define FAKECAMERAS_HPP

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "ICameras.hpp"

using namespace std;
using namespace cv;

class FakeCameras : public ICameras
{
public:
    FakeCameras();
    Mat getLeft() override;
    Mat getRight() override;
private:
    Mat left_;
    Mat right_;
    void openImages();
    const string path;
    const string leftName;
    const string rightName;
};

#endif //FAKECAMERAS_HPP