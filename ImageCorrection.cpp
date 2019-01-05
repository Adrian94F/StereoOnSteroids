#include "ImageCorrection.hpp"

ImageCorrection::ImageCorrection(string calibFilePath)
{
    readCalibrationDataFile(calibFilePath);
}

void ImageCorrection::undistortRectify(MatsPair& mats)
{
    MatsPair matsX, matsY, matsOut;

    cv::initUndistortRectifyMap(K.left, D.left, R.left, P.left, mats.left.size(), CV_32F, matsX.left, matsY.left);
    cv::initUndistortRectifyMap(K.right, D.right, R.right, P.right, mats.right.size(), CV_32F, matsX.right, matsY.right);
    cv::remap(mats.left, matsOut.left, matsX.left, matsY.left, cv::INTER_LINEAR);
    cv::remap(mats.right, matsOut.right, matsX.right, matsY.right, cv::INTER_LINEAR);

    mats = matsOut;
}

void ImageCorrection::readCalibrationDataFile(string calibFilePath)
{
    cv::FileStorage fs1(calibFilePath, cv::FileStorage::READ);
    fs1["K1"] >> K.left;
    fs1["K2"] >> K.right;
    fs1["D1"] >> D.left;
    fs1["D2"] >> D.right;
    fs1["T"] >> T;
    fs1["R1"] >> R.left;
    fs1["R2"] >> R.right;
    fs1["P1"] >> P.left;
    fs1["P2"] >> P.right;
    fs1["Q"] >> Q;
}