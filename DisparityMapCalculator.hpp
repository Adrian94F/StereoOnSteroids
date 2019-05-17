#ifndef DISPARITYMAPCALCULATOR_HPP
#define DISPARITYMAPCALCULATOR_HPP

#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/ximgproc.hpp>

#include "settings.hpp"

using namespace cv;
using namespace cv::ximgproc;

class DisparityMapCalculator
{
public:
    DisparityMapCalculator() = default;
    Mat getMap(Mat& left, Mat& right, bool filtered=true);
private:
    void calculateDisparity(Mat& left, Mat& right);
    Mat filter(Mat& mat);
private:
    int numDisparities_ = MAX_NUM_DISPARITIES;
    int blockSize_ = BLOCK_SIZE;
    int vis_mult_ = 2;
    Mat left_for_matcher, right_for_matcher, left_disp, right_disp, filtered_disp;
    Ptr<StereoMatcher> left_matcher, right_matcher;
};

#endif  // DISPARITYMAPCALCULATOR_HPP