#include <core.hpp>
#include <string>
#include "Calibration.h"
#include "Cameras.h"
#include "Preview.h"

Cameras cameras;
Calibration calibration;
Preview preview(cameras);

int main(int argc, char** argv)
{
	int choice = 2;
	system("cls");
	std::cout << 
		"Say hello to my little friend!\n" <<
		" 1 - preview & getting images\n" <<
		" 2 - calibration\n" <<
		" 0 - exit\n";
	//cin >> choice;
	switch (choice)
	{
	case 0:
		return 0;
	case 1:
		std::cout << "Press any key to capture pair of images, when windows appear. Press ESC to stop.";
		preview.start();
		break;
	case 2:
		calibration.start();
		break;
	default:
		break;
	}
	return 0;
}