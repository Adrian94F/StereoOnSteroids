#include "Preview.h"

Preview::Preview(Cameras cameras_) : cameras{cameras_}
{}

Preview::~Preview()
{}

void Preview::start()
{
	while (1) {
		concatenatedFrames = cameras.getConcatenatedFrames();
		//frames = cameras.getFrames();
		cv::imshow("Cameras", concatenatedFrames);
		/*char nr = '0';
		for (auto& frame : frames)
			cv::imshow(std::string("Camera #") + nr++, frame);*/
		if (cv::waitKey(10) == ESC)
			break;  // stop capturing by pressing ESC 
	}
}