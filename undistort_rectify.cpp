#include "header.hpp"
#include "ImageCorrection.hpp"

using namespace std;
using namespace cv;

int main(int argc, char const *argv[])
{
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    string l0, r0, l1, r1;
    cout << "Processing images [";
    for (auto i = 0; i < numOfImages; i++)
    {
        l0 = l1 = r0 = r1 = "";
        l0.append(imPath).append(nameL).append(to_string(i)).append(ext);
        l1.append(imPath).append(nameL).append(to_string(i)).append(ext);
        r0.append(imPath).append(nameR).append(to_string(i)).append(".out").append(ext);
        r1.append(imPath).append(nameR).append(to_string(i)).append(".out").append(ext);

        mats.left = imread(l0, IMREAD_COLOR);
        mats.right = imread(r0, IMREAD_COLOR);

        ic.undistortRectify(mats);

        imwrite(l1, mats.left);
        imwrite(r1, mats.right);

        cout << "#";
    }
    cout << "]\n Images saved in " << imPath << "\n";
    return 0;
}
