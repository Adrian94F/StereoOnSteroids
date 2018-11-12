#include "header.h"
#include <sys/stat.h>

vector<vector<Point3f>> objectPoints;
vector<vector<Point2f>> imagePoints;

Mat img;

void setupCalibration(string fileName)
{
    cout << " Found corners [";
    vector<Point2f> corners;
    Mat gray;
    Size boardSize = Size(boardWidth, boardHeight);
    int boardN = boardWidth * boardHeight;

    for (int k = 0; k < numOfImages; k++)
    {
        img = imread(imPath + fileName + to_string(k) + ext, CV_LOAD_IMAGE_COLOR);
        cv::cvtColor(img, gray, CV_BGR2GRAY);

        bool found = false;
        found = cv::findChessboardCorners(img, boardSize, corners,
                                          CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
        if (found)
        {
            cornerSubPix(gray, corners, cv::Size(5, 5), cv::Size(-1, -1),
                         TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
            drawChessboardCorners(gray, boardSize, corners, found);
        }

        vector<Point3f> obj;
        for (int i = 0; i < boardHeight; i++)
            for (int j = 0; j < boardWidth; j++)
                obj.push_back(Point3f((float) j * squareSize, (float) i * squareSize, 0));

        if (found)
        {
            cout << "#";
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
        }
        else
        {
            cout << " ";
        }
    }
    cout << "]\n";
}

double computeReprojectionErrors(const vector<vector<Point3f>>& objectPoints,
                                 const vector<vector<Point2f>>& imagePoints,
                                 const vector<Mat>& rvecs,
                                 const vector<Mat>& tvecs,
                                 const Mat& cameraMatrix,
                                 const Mat& distCoeffs)
{
    vector<Point2f> imagePoints2;
    int i, totalPoints = 0;
    double totalErr = 0, err;
    vector<float> perViewErrors;
    perViewErrors.resize(objectPoints.size());

    for (i = 0; i < (int) objectPoints.size(); ++i)
    {
        projectPoints(Mat(objectPoints[i]), rvecs[i],
                      tvecs[i], cameraMatrix,
                      distCoeffs, imagePoints2);
        err = norm(Mat(imagePoints[i]), Mat(imagePoints2), CV_L2);
        int n = (int) objectPoints[i].size();
        perViewErrors[i] = (float) std::sqrt(err * err / n);
        totalErr += err * err;
        totalPoints += n;
    }
    return std::sqrt(totalErr / totalPoints);
}

void calibrate(string fileName)
{
    cout << "Starting calibration for " << fileName << " camera\n";

    setupCalibration(fileName);

    Mat K;
    Mat D;
    vector<Mat> rvecs, tvecs;
    int flag = 0;
    flag |= CV_CALIB_FIX_K4;
    flag |= CV_CALIB_FIX_K5;
    calibrateCamera(objectPoints, imagePoints, img.size(), K, D, rvecs, tvecs, flag);

    cout << " Calibration error: " << computeReprojectionErrors(objectPoints, imagePoints, rvecs, tvecs, K, D) << endl;

    FileStorage fs(calibPath + fileName + ".yaml", FileStorage::WRITE);
    fs << "K" << K;
    fs << "D" << D;
    fs << "boardWidth" << boardWidth;
    fs << "boardHeight" << boardHeight;
    fs << "squareSize" << squareSize;
    printf(" Done calibration\n");
}

int main()
{
    calibrate(nameL);
    calibrate(nameR);
}