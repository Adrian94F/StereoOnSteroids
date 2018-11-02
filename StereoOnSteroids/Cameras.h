#include <core.hpp>
#include <highgui.hpp>
#include <imgcodecs.hpp>
#include <imgproc.hpp>
#include <flann/logger.h>
#include <vector>

#pragma once

using logger = cvflann::Logger;

class Cameras
{
private:
	std::vector <cv::VideoCapture> cameras;
	std::vector <cv::Mat> camFrames;
	std::vector <cv::Mat> imFrames;
	int resizeRate;
	int beginIndex;
	void capture();
	void resize();
public:
	Cameras(int numOfCameras_ = 2,
		int resizeRate_ = 0.5,
		int beginIndex_ = 1);
	std::vector <cv::Mat> getFrames();
	cv::Mat getConcatenatedFrames();
	~Cameras();
};

