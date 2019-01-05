#include "header.h"

vector<vector<Point3f>> objectPoints;
vector<vector<Point2f>> imagePoints1, imagePoints2;
vector<Point2f> corners1, corners2;
vector<vector<Point2f>> left_img_points, right_img_points;

Mat img1, img2, gray1, gray2;

void load_image_points(int board_width,
                        int board_height,
                        int num_imgs,
                        float square_size,
                        string leftimg_dir,
                        string rightimg_dir,
                        string leftimg_filename,
                        string rightimg_filename)
{

    Size board_size = Size(board_width, board_height);

    cout << " Found corners [";
    for (int i = 0; i < num_imgs; i++)
    {

        img1 = imread(imPath + nameL + to_string(i) + ext, IMREAD_COLOR);
        img2 = imread(imPath + nameR + to_string(i) + ext, IMREAD_COLOR);
        cvtColor(img1, gray1, COLOR_BGR2GRAY);
        cvtColor(img2, gray2, COLOR_BGR2GRAY);

        bool found1 = false, found2 = false;

        found1 = cv::findChessboardCorners(img1, board_size, corners1,
    CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FILTER_QUADS);
        found2 = cv::findChessboardCorners(img2, board_size, corners2,
    CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_FILTER_QUADS);


        if(!found1 || !found2)
        {
            cout << " ";
            continue;
        } 

        cv::cornerSubPix(gray1, corners1, cv::Size(5, 5), cv::Size(-1, -1),
                cv::TermCriteria(/*CV_TERMCRIT_EPS | CV_TERMCRIT_ITER*/ 0, 30, 0.1));
        cv::drawChessboardCorners(gray1, board_size, corners1, found1);

        cv::cornerSubPix(gray2, corners2, cv::Size(5, 5), cv::Size(-1, -1),
                cv::TermCriteria(/*CV_TERMCRIT_EPS | CV_TERMCRIT_ITER*/ 0, 30, 0.1));
        cv::drawChessboardCorners(gray2, board_size, corners2, found2);


        vector< Point3f > obj;
        for (int i = 0; i < board_height; i++)
            for (int j = 0; j < board_width; j++)
                obj.push_back(Point3f((float)j * square_size, (float)i * square_size, 0));

        cout << "#";
        imagePoints1.push_back(corners1);
        imagePoints2.push_back(corners2);
        objectPoints.push_back(obj);
    }
    cout << "]\n";
    for (int i = 0; i < imagePoints1.size(); i++)
    {
        vector< Point2f > v1, v2;
        for (int j = 0; j < imagePoints1[i].size(); j++)
        {
            v1.push_back(Point2f((double)imagePoints1[i][j].x, (double)imagePoints1[i][j].y));
            v2.push_back(Point2f((double)imagePoints2[i][j].x, (double)imagePoints2[i][j].y));
        }
        left_img_points.push_back(v1);
        right_img_points.push_back(v2);
    }
}

int main(int argc, char const *argv[])
{

    FileStorage fsl(calibPath + nameL + ".yaml", FileStorage::READ);
    FileStorage fsr(calibPath + nameR + ".yaml", FileStorage::READ);

    load_image_points(fsl["boardWidth"],
                        fsl["boardHeight"],
                        numOfImages,
                        fsl["squareSize"],
                        imPath,
                        imPath,
                        nameL,
                        nameR);

    printf(" Starting stereo calibration\n");
    Mat K1, K2, R, F, E;
    Vec3d T;
    Mat D1, D2;
    fsl["K"] >> K1;
    fsr["K"] >> K2;
    fsl["D"] >> D1;
    fsr["D"] >> D2;
    int flag = 0;
    flag |= CALIB_FIX_INTRINSIC;
    
    cout << " Read intrinsics" << endl;
    
    stereoCalibrate(objectPoints, left_img_points, right_img_points, K1, D1, K2, D2, img1.size(), R, T, E, F);

    cv::FileStorage fs1(calibPath + calibFile, cv::FileStorage::WRITE);
    fs1 << "K1" << K1;
    fs1 << "K2" << K2;
    fs1 << "D1" << D1;
    fs1 << "D2" << D2;
    fs1 << "R" << R;
    fs1 << "T" << T;
    fs1 << "E" << E;
    fs1 << "F" << F;
    
    printf(" Done Calibration\n");

    printf(" Starting Rectification\n");

    cv::Mat R1, R2, P1, P2, Q;
    stereoRectify(K1, D1, K2, D2, img1.size(), R, T, R1, R2, P1, P2, Q);

    fs1 << "R1" << R1;
    fs1 << "R2" << R2;
    fs1 << "P1" << P1;
    fs1 << "P2" << P2;
    fs1 << "Q" << Q;

    printf(" Done Rectification\n");

    return 0;
}
