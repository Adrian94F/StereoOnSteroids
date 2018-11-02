#include "Calibration.h"

Calibration::Calibration()
{
	for (auto& name : names_)
	{
		intrinsicCalibrations_.push_back(IntrinsicCalibration(name));
	}
}

Calibration::~Calibration()
{}

void Calibration::start()
{
	system("cls");
	std::cout << "### Intrinsic calibration of a single cameras\n";
	for (auto& calibration : intrinsicCalibrations_)
	{
		calibration.start();
	}

	std::cout << "### Stereo calibration for extrinisics\n";
	//TODO

	std::cout << "### Undistortion and Rectification\n";
	//TODO

	system("pause");
}