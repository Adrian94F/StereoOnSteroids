#include "FakeCameras.hpp"

FakeCameras::FakeCameras()
: path{"../"}
, leftName{"left.jpg"}
, rightName{"right.jpg"}
{
    openImages();
}

void FakeCameras::openImages()
{
    //Mat left, right;
    left_ = imread(path + leftName, IMREAD_COLOR);
    right_ = imread(path + rightName, IMREAD_COLOR);
    if (left_.cols == 0 || left_.cols != right_.cols)
        throw "FakeCameras error";
}

Mat FakeCameras::getLeft()
{
    return left_;
}

Mat FakeCameras::getRight()
{
    return right_;
}