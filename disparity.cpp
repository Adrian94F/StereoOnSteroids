#include "header.hpp"

#include <opencv2/ximgproc.hpp>

#include "Cameras.hpp"
#include "DisparityMapCalculator.hpp"
#include "ImageCorrection.hpp"

using namespace cv::ximgproc;

int test()
{
    DisparityMapCalculator dmc;
    imPath = "../calib_imgs/opencvdoc/";
    Mat left  = imread(imPath + nameL + "0.out" + ext, IMREAD_COLOR);
    if ( left.empty() )
    {
        cout << "Cannot read image file";
        return -1;
    }
    Mat right  = imread(imPath + nameR + "0.out" + ext, IMREAD_COLOR);
    if ( right.empty() )
    {
        cout << "Cannot read image file";
        return -1;
    }
    namedWindow("raw disparity", WINDOW_AUTOSIZE);
    namedWindow("filtered disparity", WINDOW_AUTOSIZE);
    do
    {
        imshow("filtered disparity", dmc.getMap(left, right));
        imshow("raw disparity", dmc.getMap(left, right, false));
    } while(waitKey(30) != 27);

    return 0;
}

void liveProcessing()
{
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Cameras cameras;
    DisparityMapCalculator dmc;
    do
    {
        mats.left = cameras.getLeft();
        mats.right = cameras.getRight();
        ic.undistortRectify(mats);
        Mat map = dmc.getMap(mats.left, mats.right);
        imshow("Cameras", cameras.getBoth());
        imshow("Disparity", dmc.getMap(mats.left, mats.right));
    } while(waitKey(30) != 27);
}

int main(int argc, char const *argv[])
{
    /*if (test())
        return 1;*/
    liveProcessing();
    return 0;
}