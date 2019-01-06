#include "header.hpp"

#include "Cameras.hpp"

int main(int argc, char const *argv[])
{
    Cameras cameras;

    cout << "Press ESC to close program. Press any key to save images pair.\n";
    int x = 0;
    while (true)
    {
        imshow("Preview", cameras.getBoth());
        auto key = waitKey(30);
        if (key > 0)
        {
            if (key == 27)
            {
                return 0;
            }
            cout << "Saving img pair " << x << endl;
            imwrite(imPath + nameL + to_string(x) + ext, cameras.getLeft());
            imwrite(imPath + nameR + to_string(x) + ext, cameras.getRight());
            x++;
        }
    }
}
