#include "header.h"

bool checkCameras(VideoCapture c1, VideoCapture c2)
{
    if (!c1.isOpened())
    {
        cout << "Cam 1 error" << endl;
        return true;
    }
    if (!c2.isOpened())
    {
        cout << "Cam 2 error" << endl;
        return true;
    }
    return false;
}


int main(int argc, char const *argv[])
{
    VideoCapture cap1;
    VideoCapture cap2;
    cap1.open(0);
    cap2.open(2);
    Mat img1, img2;

    if (checkCameras(cap1, cap2))
        return 1;

    int x = 0;
    while (1)
    {
        cap1 >> img1;
        cap2 >> img2;
        imshow("Right", img1);
        imshow("Left", img2);
        auto key = waitKey(30);
        if (key > 0)
        {
            if (key == 27)
            {
                return 0;
            }
            cout << "Saving img pair " << x << endl;
            imwrite(imPath + nameL + to_string(x) + ext, img1);
            imwrite(imPath + nameR + to_string(x) + ext, img2);
            x++;
        }
    }
    return 0;
}
