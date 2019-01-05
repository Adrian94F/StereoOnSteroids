#include "header.h"

#include <opencv2/ximgproc.hpp>

using namespace cv::ximgproc;

int test()
{
    int max_disp = 64;
    int wsize = 7;
    int vis_mult = 2;
    int i = 0;
    double matching_time, filtering_time;
    Mat left, right, left_for_matcher, right_for_matcher, left_disp, right_disp, filtered_disp;

    //left  = imread("../left.jpg", IMREAD_COLOR);
    left  = imread(imPath + nameL + to_string(i) + ".out" + ext, IMREAD_COLOR);
    if ( left.empty() )
    {
        cout << "Cannot read image file";
        return -1;
    }
    //right = imread("../right.jpg", IMREAD_COLOR);
    right  = imread(imPath + nameR + to_string(i) + ".out" + ext, IMREAD_COLOR);
    if ( right.empty() )
    {
        cout << "Cannot read image file";
        return -1;
    }

    max_disp/=2;
    if(max_disp%16!=0)
        max_disp += 16-(max_disp%16);
    resize(left ,left_for_matcher ,Size(),0.5,0.5);
    resize(right,right_for_matcher,Size(),0.5,0.5);

    Ptr<StereoBM> left_matcher = StereoBM::create(max_disp,wsize);
    auto wls_filter = createDisparityWLSFilter(left_matcher);
    Ptr<StereoMatcher> right_matcher = createRightMatcher(left_matcher);
    cvtColor(left_for_matcher,  left_for_matcher,  COLOR_BGR2GRAY);
    cvtColor(right_for_matcher, right_for_matcher, COLOR_BGR2GRAY);
    matching_time = (double)getTickCount();
    left_matcher-> compute(left_for_matcher, right_for_matcher,left_disp);
    right_matcher->compute(right_for_matcher,left_for_matcher, right_disp);
    matching_time = ((double)getTickCount() - matching_time)/getTickFrequency();

    //wls_filter->setLambda(lambda);
    //wls_filter->setSigmaColor(sigma);
    filtering_time = (double)getTickCount();
    wls_filter->filter(left_disp,left,filtered_disp,right_disp);
    filtering_time = ((double)getTickCount() - filtering_time)/getTickFrequency();

    Mat raw_disp_vis;
    cv::ximgproc::getDisparityVis(left_disp, raw_disp_vis, vis_mult);
    namedWindow("raw disparity", WINDOW_AUTOSIZE);
    Mat filtered_disp_vis;
    cv::ximgproc::getDisparityVis(filtered_disp, filtered_disp_vis, vis_mult);
    namedWindow("filtered disparity", WINDOW_AUTOSIZE);
    do
    {
        imshow("raw disparity", raw_disp_vis);
        imshow("filtered disparity", filtered_disp_vis);
    } while(waitKey(30) != 27);

    return 0;
}

void liveProcessing()
{
    do
    {
        //capture frames
        //undostort_rectify
        //get disparity map
        //view
    } while(waitKey(30) != 27);
}

int main(int argc, char const *argv[])
{
    if (test())
        return 1;
    liveProcessing();
    return 0;
}