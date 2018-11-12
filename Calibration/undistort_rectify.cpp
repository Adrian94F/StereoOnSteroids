#include "header.h"

using namespace std;
using namespace cv;


int main(int argc, char const *argv[])
{
    cout << "Processing images [";
    for (auto i = 0; i < numOfImages; i++)
    {
        Mat R1, R2, P1, P2, Q;
        Mat K1, K2, R;
        Vec3d T;
        Mat D1, D2;

        Mat img1 = imread(imPath + nameL + to_string(i) + ext, CV_LOAD_IMAGE_COLOR);
        Mat img2 = imread(imPath + nameR + to_string(i) + ext, CV_LOAD_IMAGE_COLOR);

        cv::FileStorage fs1(calibPath + calibFile, cv::FileStorage::READ);
        fs1["K1"] >> K1;
        fs1["K2"] >> K2;
        fs1["D1"] >> D1;
        fs1["D2"] >> D2;
        fs1["R"] >> R;
        fs1["T"] >> T;

        fs1["R1"] >> R1;
        fs1["R2"] >> R2;
        fs1["P1"] >> P1;
        fs1["P2"] >> P2;
        fs1["Q"] >> Q;

        cv::Mat lmapx, lmapy, rmapx, rmapy;
        cv::Mat imgU1, imgU2;

        cv::initUndistortRectifyMap(K1, D1, R1, P1, img1.size(), CV_32F, lmapx, lmapy);
        cv::initUndistortRectifyMap(K2, D2, R2, P2, img2.size(), CV_32F, rmapx, rmapy);
        cv::remap(img1, imgU1, lmapx, lmapy, cv::INTER_LINEAR);
        cv::remap(img2, imgU2, rmapx, rmapy, cv::INTER_LINEAR);

        imwrite(imPath + nameL + to_string(i) + ".out" + ext, imgU1);
        imwrite(imPath + nameR + to_string(i) + ".out" + ext, imgU2);

        cout << "#";
    }
    cout << "]\n";
    return 0;
}
