#include "Cameras.h"

Cameras::Cameras(Resolution camResolution_, Resolution imResolution_, int numOfCameras_, int beginIndex_) 
	: camResolution{ camResolution_ }
	, imResolution{ imResolution_ }
	, beginIndex{ beginIndex_ }
{
	for (auto i = beginIndex; i < beginIndex + numOfCameras_; i++)
	{
		cv::VideoCapture cam;
		if (!cam.open(i))
		{
			logger::fatal("something went wrong\n");
			return;
		}
		cam.set(cv::CAP_PROP_FRAME_WIDTH, camResolution.width);
		cam.set(cv::CAP_PROP_FRAME_HEIGHT, camResolution.height);
		int h = cam.get(cv::CAP_PROP_FRAME_HEIGHT);
		int w = cam.get(cv::CAP_PROP_FRAME_WIDTH);
		if (h != 720 || w != 1280)
		{
			logger::fatal("unable to set 1280*720\n");
			return;
		}
		cameras.push_back(cam);
		camFrames.push_back(cv::Mat());
		imFrames.push_back(cv::Mat());
	}
}

Cameras::~Cameras()
{
}

void Cameras::capture()
{
	for (auto i = 0; i < cameras.size(); i++)
		cameras[i] >> camFrames[i];
}

void Cameras::resize()
{
	for (auto i = 0; i < cameras.size(); i++)
		cv::resize(camFrames[i], imFrames[i], cv::Size(imResolution.width, imResolution.height));
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