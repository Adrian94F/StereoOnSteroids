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
	typedef struct
	{
		int width;
		int height;
	} Resolution;
	std::vector <cv::VideoCapture> cameras;
	std::vector <cv::Mat> camFrames;
	std::vector <cv::Mat> imFrames;
	Resolution camResolution;
	Resolution imResolution;
	int beginIndex;
	void capture();
	void resize();
public:
	Cameras(Resolution camResolution_ = {1280, 720},
		Resolution imResolution_ = {640, 360},
		int numOfCameras_ = 2,
		int beginIndex_ = 1);
	std::vector <cv::Mat> getFrames();
	cv::Mat getConcatenatedFrames();
	~Cameras();
};

