#include <boost/optional.hpp>
#include <condition_variable>
#include <mutex>
#include <opencv2/ximgproc.hpp>
#include <thread>

#include "FakeCameras.hpp"
#include "header.hpp"
#include "DisparityMapCalculator.hpp"
#include "ImageCorrection.hpp"
#include "Task.hpp"
#include "Timer.hpp"

using namespace cv::ximgproc;

int numberOfTasks = 4;

std::vector<cv::Mat> lInputs(numberOfTasks);
std::vector<cv::Mat> rInputs(numberOfTasks);
std::vector<cv::Mat> results(numberOfTasks);

std::vector<Task> tasksToDo(numberOfTasks);

void singleThreadProcessing()
{
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    FakeCameras cameras;
    DisparityMapCalculator dmc;
    Timer timer("SingleThreadDisparity");
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
        imshow("Cameras", ICameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", map);
        timer.measure(Timer::EMeasure::FRAME_END);
    } while(waitKey(30) != 27);
    timer.printLog();
}

void masterTask()
{
    FakeCameras cameras;
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Timer timer("MultiThreadedDisparity");

    DisparityMapCalculator dmc;

    timer.reset();
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
            lInputs[i] = cv::Mat(mats.left, cv::Rect(0, i * partHeight, width, partHeight));
            rInputs[i] = cv::Mat(mats.right, cv::Rect(0, i * partHeight, width, partHeight));
            tasksToDo[i] = {Task::ETaskStatus_TODO};  //, i};
        }

        // wait for map generation in slaves
        while(std::count_if(
                tasksToDo.begin(), tasksToDo.end(),
                [](auto task){ return task.status == Task::ETaskStatus_Done; }
            ) < numberOfTasks);

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
        if (map.cols > 0 && map.rows > 0)
        {
            imshow("Disparity", map);
        }
        timer.measure(Timer::EMeasure::FRAME_END);
    } while(waitKey(30) != 27);

    for (auto i = 0; i < numberOfTasks; i++)
    {
        tasksToDo[i] = {Task::ETaskStatus_Abort};
    }
    timer.printLog();
}

void slaveTask(int id)
{
    DisparityMapCalculator dmc;

    while(tasksToDo[id].status == Task::ETaskStatus_Wait);

    while(tasksToDo[id].status != Task::ETaskStatus_Abort)
    {
        while(tasksToDo[id].status != Task::ETaskStatus_TODO && tasksToDo[id].status != Task::ETaskStatus_Abort);
        if (tasksToDo[id].status == Task::ETaskStatus_Abort)
        {
            break;
        }

        std::cout << id << ' ';
        results[id] = dmc.getMap(lInputs[id], rInputs[id]);

        tasksToDo[id].status = Task::ETaskStatus_Done;

    }
}

void parallelProcessing()
{
    std::vector<std::thread> threads;
    threads.resize(numberOfTasks);

    for (auto i = 0; i < numberOfTasks; i++)
    {
        tasksToDo[i] = {Task::ETaskStatus_Wait};
    }

    for (auto t = 0; t < threads.size(); t++)
    {
        threads[t] = std::thread(slaveTask, t);
    }

    masterTask();

    for (auto t = 0; t < threads.size(); t++)
    {
        threads[t].native_handle();
        threads[t].join();
    }
}

int main(int argc, char const *argv[])
{
    //singleThreadProcessing();
    parallelProcessing();
    return 0;
}