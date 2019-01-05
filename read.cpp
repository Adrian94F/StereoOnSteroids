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

Mat resizeAndConcat(Mat imL, Mat imR, int d)
{
    resize(imL, imL, Size(imL.cols / d, imL.rows / d));
    resize(imR, imR, Size(imR.cols / d, imR.rows / d));
    Mat result = imL;
    hconcat(result, imR, result);
    return result;
}

int main(int argc, char const *argv[])
{
    VideoCapture capL;
    VideoCapture capR;
    capL.open(2);
    capR.open(0);
    Mat imgL, imgR, imgLR;

    if (checkCameras(capL, capR))
        return 1;

    cout << "Press ESC to close program. Press any key to save images pair.\n";
    int x = 0;
    while (true)
    {
        capL >> imgL;
        capR >> imgR;
        imgLR = resizeAndConcat(imgL, imgR, 2);
        imshow("Preview", imgLR);
        //imshow("Left", imgL);
        //imshow("Right", imgR);
        auto key = waitKey(30);
        if (key > 0)
        {
            if (key == 27)
            {
                return 0;
            }
            cout << "Saving img pair " << x << endl;
            imwrite(imPath + nameL + to_string(x) + ext, imgL);
            imwrite(imPath + nameR + to_string(x) + ext, imgR);
            x++;
        }
    }
}
