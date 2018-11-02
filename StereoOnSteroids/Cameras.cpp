#include "Cameras.h"

Cameras::Cameras(int numOfCameras_, int resizeRate_, int beginIndex_) 
	: resizeRate{ resizeRate_ }, beginIndex{ beginIndex_ }
{
	for (auto i = beginIndex; i < beginIndex + numOfCameras_; i++)
	{
		cv::VideoCapture cam;
		if (!cam.open(i))
		{
			logger::fatal("something went wrong\n");
			return;
		}
		cameras.push_back(cam);
		camFrames.push_back(cv::Mat());
		imFrames.push_back(cv::Mat());
	}
}

Cameras::~Cameras()
{}

void Cameras::capture()
{
	for (auto i = 0; i < cameras.size(); i++)
		cameras[i] >> camFrames[i];
}

void Cameras::resize()
{
	for (auto i = 0; i < cameras.size(); i++)
		//imFrames[i] = camFrames[i];
		cv::resize(camFrames[i], imFrames[i], cv::Size(camFrames[i].cols, camFrames[i].rows));
}

std::vector <cv::Mat> Cameras::getFrames()
{
	capture();
	resize();
	return imFrames;
}

cv::Mat Cameras::getConcatenatedFrames()
{
	getFrames();
	cv::Mat out = imFrames[0];
	for (auto i = 1; i < imFrames.size(); i++)
		hconcat(out, imFrames[i], out);
	return out;
}