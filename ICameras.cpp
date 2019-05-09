#include "ICameras.hpp"


Mat ICameras::resizeAndConcat(Mat imL, Mat imR, int d)
{
    resize(imL, imL, Size(imL.cols / d, imL.rows / d));
    resize(imR, imR, Size(imR.cols / d, imR.rows / d));
    Mat result = imL;
    hconcat(result, imR, result);
    return result;
}

Mat ICameras::getBoth()
{
    return resizeAndConcat(getLeft(), getRight());
}