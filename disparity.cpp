#include "header.hpp"

#include <opencv2/ximgproc.hpp>

#include "Cameras.hpp"
#include "DisparityMapCalculator.hpp"
#include "ImageCorrection.hpp"
#include "Timer.hpp"

using namespace cv::ximgproc;

void liveProcessing()
{
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Cameras cameras;
    DisparityMapCalculator dmc;
    Timer timer("Disparity");
    timer.reset();
    do
    {
        timer.measure("[");
        mats.left = cameras.getLeft();
        mats.right = cameras.getRight();
        timer.measure(" got images");
        ic.undistortRectify(mats);
        timer.measure(" undistorted");
        Mat map = dmc.getMap(mats.left, mats.right);
        timer.measure(" got disparity map");
        imshow("Cameras", Cameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", map);
        timer.measure("]");
    } while(waitKey(30) != 27);
    timer.printLog();
}

int main(int argc, char const *argv[])
{
    /*if (test())
        return 1;*/
    liveProcessing();
    return 0;
}