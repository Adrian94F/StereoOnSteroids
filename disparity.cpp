#include "header.hpp"

#include <opencv2/ximgproc.hpp>

#include "Cameras.hpp"
#include "DisparityMapCalculator.hpp"
#include "ImageCorrection.hpp"

using namespace cv::ximgproc;

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
        imshow("Cameras", Cameras::resizeAndConcat(mats.left, mats.right));
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