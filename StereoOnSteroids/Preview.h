#pragma once

#include "Cameras.h"

#define ESC 27

class Preview
{
public:
	Preview(Cameras);
	~Preview();
	void start();
private:
	Cameras cameras;
	cv::Mat concatenatedFrames;
};