#include "IntrinsicCalibration.h"

IntrinsicCalibration::IntrinsicCalibration(std::string name) : fileName_{ name }
{}

IntrinsicCalibration::~IntrinsicCalibration()
{}

void IntrinsicCalibration::start()
{
	std::cout << " Calibration of " << imgsDirectory_ + fileName_ + "*." + extension_ << std::endl;
	setup();
	calibrate();
}

void IntrinsicCalibration::setup()
{
	cv::Size board_size = cv::Size(boardWidth_, boardHeight_);
	int board_n = boardWidth_ * boardHeight_;

	std::cout << "  Found corners in images [";
	for (auto k = 1; k <= numImgs_; k++)
	{
		char img_file[100];
		sprintf_s(img_file,
			"%s%s%d.%s",
			imgsDirectory_.c_str(),
			fileName_.c_str(),
			k,
			extension_.c_str());
		img_ = cv::imread(img_file, CV_LOAD_IMAGE_COLOR);
		cv::cvtColor(img_, gray_, CV_BGR2GRAY);

		bool found = false;
		found = cv::findChessboardCorners(img_, board_size, corners_,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		if (found)
		{
			cornerSubPix(gray_, corners_, cv::Size(5, 5), cv::Size(-1, -1),
				cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners(gray_, board_size, corners_, found);
		}

		std::vector< cv::Point3f > obj;
		for (auto i = 0; i < boardHeight_; i++)
			for (auto j = 0; j < boardWidth_; j++)
				obj.push_back(cv::Point3f((float)j * squareSize_, (float)i * squareSize_, 0));

		if (found)
		{
			std::cout << '#';
			imagePoints_.push_back(corners_);
			objectPoints_.push_back(obj);
		}
		else
		{
			std::cout << ' ';
		}
	}
	std::cout << "]\n";
}

double IntrinsicCalibration::computeReprojectionErrors(
	const std::vector<cv::Mat>& rvecs,
	const std::vector<cv::Mat>& tvecs,
	const cv::Mat& cameraMatrix,
	const cv::Mat& distCoeffs)
{
	std::vector<cv::Point2f> imagePoints2;
	int totalPoints = 0;
	double totalErr = 0;
	double err;
	std::vector< float > perViewErrors;
	perViewErrors.resize(objectPoints_.size());

	for (auto i = 0; i < (int)objectPoints_.size(); ++i)
	{
		projectPoints(cv::Mat(objectPoints_[i]),
			rvecs[i],
			tvecs[i],
			cameraMatrix,
			distCoeffs,
			imagePoints2);
		err = cv::norm(cv::Mat(imagePoints_[i]), cv::Mat(imagePoints2), CV_L2);
		int n = (int)objectPoints_[i].size();
		perViewErrors[i] = (float)std::sqrt(err * err / n);
		totalErr += err * err;
		totalPoints += n;
	}
	return std::sqrt(totalErr / totalPoints);
}

void IntrinsicCalibration::calibrate()
{
	std::cout << "  Starting calibration...";
	cv::Mat K;
	cv::Mat D;
	std::vector< cv::Mat > rvecs, tvecs;
	int flag = 0;
	flag |= CV_CALIB_FIX_K4;
	flag |= CV_CALIB_FIX_K5;
	calibrateCamera(objectPoints_, imagePoints_, img_.size(), K, D, rvecs, tvecs, flag);

	std::cout << "  Calibration error: " << computeReprojectionErrors(rvecs, tvecs, K, D) << std::endl;

	cv::FileStorage fs(fileName_ + ".yaml", cv::FileStorage::WRITE);
	fs << "K" << K;
	fs << "D" << D;
	fs << "boardWidth" << boardWidth_;
	fs << "boardHeight" << boardHeight_;
	fs << "squareSize" << squareSize_;
	std::cout << "  Calibration done!\n";
	return;
}