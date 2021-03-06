#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <iostream>
#include <stdio.h>

using namespace std;
using namespace cv;

int boardWidth = 9;
int boardHeight = 6;
float squareSize = 0.0265;
int numOfImages = 36;
string imPath = "../calib_imgs/final/";

string nameL = "left";
string nameR = "right";
string ext = ".jpg";

string calibPath = "../";
string calibFile = "calibStereo.yaml";