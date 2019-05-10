#include <condition_variable>
#include <mutex>
#include <opencv2/ximgproc.hpp>
#include <thread>

#include "FakeCameras.hpp"
#include "header.hpp"
#include "DisparityMapCalculator.hpp"
#include "ImageCorrection.hpp"
#include "Timer.hpp"

using namespace cv::ximgproc;

int numberOfTasks = 4;

std::vector<cv::Mat> lInputs(numberOfTasks);
std::vector<cv::Mat> rInputs(numberOfTasks);
std::vector<cv::Mat> results(numberOfTasks);

bool start = false;
bool active = true;
std::vector<std::mutex> ready(numberOfTasks);
std::vector<std::mutex> processed(numberOfTasks);

void singleThreadProcessing()
{
    /*ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Cameras cameras;
    DisparityMapCalculator dmc;
    Timer timer("Disparity");
    timer.reset();
    do
    {
        timer.measure(Timer::EMeasure::FRAME_BEGIN);
        mats.left = cameras.getLeft();
        mats.right = cameras.getRight();
        timer.measure(Timer::EMeasure::GOT_IMAGES);
        ic.undistortRectify(mats);
        timer.measure(Timer::EMeasure::UNDISTORTED);
        Mat map = dmc.getMap(mats.left, mats.right);
        timer.measure(Timer::EMeasure::DISPARITY_MAP_GENERATED);
        imshow("Cameras", Cameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", map);
        timer.measure(Timer::EMeasure::FRAME_END);
    } while(waitKey(30) != 27);
    timer.printLog();*/
}

void masterTask()
{
    FakeCameras cameras;
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Timer timer("Disparity");

    DisparityMapCalculator dmc;

    timer.reset();

    for (auto i = 0; i < numberOfTasks; i++)
    {
        processed[i].lock();
        ready[i].lock();
    }
    start = true;
    do
    {
        // get images from cameras
        timer.measure(Timer::EMeasure::FRAME_BEGIN);
        mats.left = cameras.getLeft();
        mats.right = cameras.getRight();
        timer.measure(Timer::EMeasure::GOT_IMAGES);
        // transform images
        ic.undistortRectify(mats);
        timer.measure(Timer::EMeasure::UNDISTORTED);

        // divide into parts for each slave
        int width = mats.left.cols;
        int height = mats.left.rows;
        int partHeight = height / numberOfTasks;

        cv::Mat map;

        for (auto i = 0; i < numberOfTasks; i++)
        {
            processed[i].unlock();
            lInputs[i] = cv::Mat(mats.left, cv::Rect(0, i * partHeight, width, partHeight));
            rInputs[i] = cv::Mat(mats.right, cv::Rect(0, i * partHeight, width, partHeight));

            ready[i].unlock();
        }

        // wait for map generation in slaves
        for (auto i = 0; i < numberOfTasks; i++)
        {
            processed[i].lock();
            ready[i].lock();
        }

        // merge images
        for (auto i = 0; i < numberOfTasks; i++)
        {
            auto submap = results[i];
            if (i > 0)
            {
                vconcat(map, submap, map);
            }
            else
            {
                map = submap;
            }
        }
        timer.measure(Timer::EMeasure::DISPARITY_MAP_GENERATED);
        // display images
        imshow("Cameras (scale 0.5)", ICameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", map);
        timer.measure(Timer::EMeasure::FRAME_END);
    } while(waitKey(30) != 27);
    timer.printLog();
}

void slaveTask(int id)
{
    std::cout << "Slave " << id << " is ready\n";

    DisparityMapCalculator dmc;

    while(!start);
    std::cout << "Slave " << id << " started\n";

    while(active)
    {
        // Wait until master prepares data
        ready[id].lock();
        processed[id].lock();

        std::cout << "generation of submap #" << id << std::endl;
        results[id] = dmc.getMap(lInputs[id], rInputs[id]);
        if (results[id].cols == 0)
        {
            std::cerr << "fucked up\n";
        }

        ready[id].unlock();
        processed[id].unlock();

    }
}

void parallelProcessing()
{
    std::vector<std::thread> threads;
    threads.resize(numberOfTasks);

    for (auto t = 0; t < threads.size(); t++)
    {
        threads[t] = std::thread(slaveTask, t);
    }

    masterTask();
    active = false;

    for (auto t = 0; t < threads.size(); t++)
    {
        ready[t].unlock();
        processed[t].unlock();

        threads[t].native_handle();
        threads[t].join();

        cout << "joined thread " << t << endl;
    }
}

int main(int argc, char const *argv[])
{
    singleThreadProcessing();
    parallelProcessing();
    return 0;
}