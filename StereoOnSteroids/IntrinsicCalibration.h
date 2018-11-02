#pragma once

#include <iostream>
#include <stdio.h>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class IntrinsicCalibration
{
public:
	IntrinsicCalibration(std::string);
	~IntrinsicCalibration();
	void start();

private:
	void setup();
	double computeReprojectionErrors(
		const std::vector<cv::Mat>& rvecs,
		const std::vector<cv::Mat>& tvecs,
		const cv::Mat& cameraMatrix,
		const cv::Mat& distCoeffs);
	void calibrate();

	int boardWidth_ = 9;
	int boardHeight_ = 9;
	int numImgs_ = 16;
	float squareSize_ = 19.5;

	std::string imgsDirectory_ = "calib_imgs/0/";
	std::string fileName_;
	std::string extension_ = "jpg";

	std::vector<std::vector<cv::Point3f>> objectPoints_;
	std::vector<std::vector<cv::Point2f>> imagePoints_;
	std::vector<cv::Point2f> corners_;
	std::vector<std::vector<cv::Point2f>> leftImgPoints_;
	cv::Mat img_;
	cv::Mat gray_;
	cv::Size imSize_;
};