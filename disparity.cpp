#include "header.hpp"

#include <opencv2/ximgproc.hpp>

#include "DisparityMapCalculator.hpp"
#include "ImageCorrection.hpp"

using namespace cv::ximgproc;

int test()
{
    DisparityMapCalculator dmc;

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
    // read cameras calibration data
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    DisparityMapCalculator dmc;

    do
    {
        // capture frames
        Mat left, right;
        // undostort_rectify
        ic.undistortRectify(mats);
        // get disparity map
        Mat map = dmc.getMap(mats.left, mats.right);
        // view
    } while(waitKey(30) != 27);
}

int main(int argc, char const *argv[])
{
    if (test())
        return 1;
    liveProcessing();
    return 0;
}