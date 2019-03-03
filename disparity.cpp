#include <opencv2/ximgproc.hpp>

#include "Cameras.hpp"
#include "DisparityMapCalculator.hpp"
#include "ImageCorrection.hpp"
#include "TaskManager.hpp"
#include "Timer.hpp"

using namespace cv::ximgproc;

void liveProcessing()
{
    /*ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Cameras cameras;
    DisparityMapCalculator dmc;
    Timer timer("Disparity");
    timer.reset();
    do
    {
        timer.measure(Timer::EMeasure::FRAME_BEGIN);
        mats.left = cameras.getLeft();
        mats.right = cameras.getRight();
        timer.measure(Timer::EMeasure::GOT_IMAGES);
        ic.undistortRectify(mats);
        timer.measure(Timer::EMeasure::UNDISTORTED);
        Mat map = dmc.getMap(mats.left, mats.right);
        timer.measure(Timer::EMeasure::DISPARITY_MAP_GENERATED);
        imshow("Cameras", Cameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", map);
        timer.measure(Timer::EMeasure::FRAME_END);
    } while(waitKey(30) != 27);
    timer.printLog();*/
}

int main(int argc, char const *argv[])
{
    /*if (test())
        return 1;*/
    TaskManager tm;
    tm.start();
    liveProcessing();
    return 0;
}