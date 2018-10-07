#include <core.hpp>
#include "Cameras.h"

#define ESC 27

cv::Mat frames;

int main(int argc, char** argv)
{
	Cameras cameras;

	while (1) {
		frames = cameras.getConcatenatedFrames();
		cv::imshow("Cameras", frames);
		if (cv::waitKey(1) == ESC)
			break;  // stop capturing by pressing ESC 
	}
	return 0;
}