#pragma once

#include "IntrinsicCalibration.h"
#include "ExtrinsicCalibration.h"
#include "UndistortionRectification.h"

class Calibration
{
private:
	std::vector<std::string> names_ = { "left", "right" };
	std::vector<IntrinsicCalibration> intrinsicCalibrations_;
public:
	Calibration();
	~Calibration();
	void start();
};

