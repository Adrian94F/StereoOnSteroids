#ifndef IMAGECORRECTION_HPP
#define IMAGECORRECTION_HPP

#include <opencv2/calib3d.hpp>
#include <opencv2/imgproc.hpp>

using namespace std;
using namespace cv;

class ImageCorrection
{
public:
    struct {
        Mat left;
        Mat right;
    } typedef MatsPair;
public:
    ImageCorrection(const string& calibFilePath = "");
    void undistortRectify(MatsPair& mats);
private:
    void readCalibrationDataFile(const string& calibFilePath);
private:
    MatsPair R;
    MatsPair P;
    MatsPair K;
    MatsPair D;
    Mat Q;
    Vec3d T;
    bool noCorrection_;
};

#endif  // IMAGECORRECTION_HPP
