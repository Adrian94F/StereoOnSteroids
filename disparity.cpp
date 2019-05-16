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

#define DISPARITY_BORDER 6

using namespace cv::ximgproc;

std::vector<cv::Mat> lInputs;
std::vector<cv::Mat> rInputs;
std::vector<cv::Mat> results;
std::vector<std::thread> threads;
std::vector<Task> tasksToDo;

void prepareVectors(int n)
{
    lInputs.resize(n);
    rInputs.resize(n);
    results.resize(n);
    threads.resize(n);
    tasksToDo.resize(n);
}

void singleThreadProcessing()
{
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    FakeCameras cameras;
    DisparityMapCalculator dmc;
    Timer timer(Timer::EMode::SINGLETHREADED);
    timer.reset();
    do
    {
        timer.measure(Timer::EMeasure::STARTED);
        mats.left = cameras.getLeft();
        mats.right = cameras.getRight();
        timer.measure(Timer::EMeasure::GOT_IMAGES);
        ic.undistortRectify(mats);
        timer.measure(Timer::EMeasure::UNDISTORTED);
        Mat map = dmc.getMap(mats.left, mats.right);
        timer.measure(Timer::EMeasure::DISPARITY_MAP_GENERATED);
        imshow("Cameras (scale 0.5)", ICameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", map);
        timer.measure(Timer::EMeasure::DISPLAYED_RESULTS);
    } while(waitKey(30) != 27);
    timer.printStatistics();
}

void masterTask(int nOfTasks)
{
    FakeCameras cameras;
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Timer timer(Timer::EMode::MULTITHREADED);

    DisparityMapCalculator dmc;

    timer.reset();
    do
    {
        // get images from cameras
        timer.measure(Timer::EMeasure::STARTED);
        mats.left = cameras.getLeft();
        mats.right = cameras.getRight();
        timer.measure(Timer::EMeasure::GOT_IMAGES);

        // transform images
        ic.undistortRectify(mats);
        timer.measure(Timer::EMeasure::UNDISTORTED);

        // divide into parts for each slave
        auto width = mats.left.cols;
        auto height = mats.left.rows / nOfTasks;
        cv::Mat disparityMap;

        for (auto i = 0; i < tasksToDo.size(); i++)
        {
            auto topOffset = i == 0 ? 0 : DISPARITY_BORDER;
            auto bottomOffset = i != (tasksToDo.size() -1) ? DISPARITY_BORDER : 0;
            auto yBegin = i * height - topOffset;
            auto partHeight = height + topOffset + bottomOffset;
            lInputs[i] = cv::Mat(mats.left, cv::Rect(0, yBegin, width, partHeight));
            rInputs[i] = cv::Mat(mats.right, cv::Rect(0, yBegin, width, partHeight));
            tasksToDo[i] = {Task::ETaskStatus_TODO};
        }
        timer.measure(Timer::EMeasure::PREPARED_TASKS);

        // wait for disparityMap generation in slaves
        while(std::count_if(
                tasksToDo.begin(), tasksToDo.end(),
                [](auto task){ return task.status == Task::ETaskStatus_Done; }
            ) < nOfTasks);
        timer.measure(Timer::EMeasure::COMPLETED_TASKS);

        // merge images
        for (auto i = 0; i < results.size(); i++)
        {
            auto submap = results[i];
            submap = cv::Mat(submap, cv::Rect(0, DISPARITY_BORDER, submap.cols, submap.rows - 2 * DISPARITY_BORDER));
            if (i > 0)
            {
                vconcat(disparityMap, submap, disparityMap);
            }
            else
            {
                disparityMap = submap;
            }
        }
        timer.measure(Timer::EMeasure::MERGED_RESULTS);

        // display images
        imshow("Cameras (scale 0.5)", ICameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", disparityMap);
        timer.measure(Timer::EMeasure::DISPLAYED_RESULTS);
    } while(waitKey(30) != 27);

    // stop threads
    std::for_each(tasksToDo.begin(), tasksToDo.end(),
            [](auto& t){ t = {Task::ETaskStatus_Abort}; });

    // print statistics
    timer.printStatistics(nOfTasks);
}

void slaveTask(int id)
{
    DisparityMapCalculator dmc;

    // wait for master readiness
    while(tasksToDo[id].status == Task::ETaskStatus_Wait);

    while(tasksToDo[id].status != Task::ETaskStatus_Abort)
    {
        while(tasksToDo[id].status != Task::ETaskStatus_TODO && tasksToDo[id].status != Task::ETaskStatus_Abort);
        if (tasksToDo[id].status == Task::ETaskStatus_Abort)
        {
            break;
        }

        // calculate disparity map
        results[id] = dmc.getMap(lInputs[id], rInputs[id]);

        tasksToDo[id].status = Task::ETaskStatus_Done;
    }
}

void parallelProcessing(int nOfTasks = 4)
{
    prepareVectors(nOfTasks);

    std::for_each(tasksToDo.begin(), tasksToDo.end(),
            [](auto& t){ t = {Task::ETaskStatus_Wait}; });

    auto id = 0;
    std::for_each(threads.begin(), threads.end(),
            [&id](std::thread& t)
            {
                t = std::thread(slaveTask, id++);
            });

    masterTask(nOfTasks);

    std::for_each(threads.begin(), threads.end(),
            [](auto& t)
            {
                t.native_handle();
                t.join();
            });
}

int main(int argc, char const *argv[])
{
    singleThreadProcessing();
    parallelProcessing(1);
    parallelProcessing(2);
    parallelProcessing(4);
    parallelProcessing(8);
    parallelProcessing(16);
    parallelProcessing(32);
    return 0;
}