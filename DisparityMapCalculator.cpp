#include "DisparityMapCalculator.hpp"

DisparityMapCalculator::DisparityMapCalculator()
{
    max_disp/=2;
    if(max_disp%16!=0)
        max_disp += 16-(max_disp%16);
}

Mat DisparityMapCalculator::getMap(Mat left, Mat right, bool filtered)
{
    calculateDisparity(left, right);
    if (filtered)
        filter(left);
    Mat vis;
    cv::ximgproc::getDisparityVis(filtered ? filtered_disp : left_disp, vis, vis_mult);
    return vis;
}

void DisparityMapCalculator::calculateDisparity(Mat left, Mat right)
{
    resize(left, left_for_matcher, Size(), 0.5, 0.5);
    resize(right, right_for_matcher, Size(), 0.5, 0.5);
    left_matcher = StereoBM::create(max_disp, wsize);
    right_matcher = createRightMatcher(left_matcher);
    cvtColor(left_for_matcher, left_for_matcher, COLOR_BGR2GRAY);
    cvtColor(right_for_matcher, right_for_matcher, COLOR_BGR2GRAY);
    left_matcher-> compute(left_for_matcher, right_for_matcher, left_disp);
    right_matcher->compute(right_for_matcher, left_for_matcher, right_disp);
}

Mat DisparityMapCalculator::filter(Mat mat)
{
    auto wls_filter = createDisparityWLSFilter(left_matcher);
    wls_filter->filter(left_disp,mat,filtered_disp,right_disp);
    return mat;
}