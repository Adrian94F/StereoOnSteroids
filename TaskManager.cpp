#include "TaskManager.hpp"

#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

#include <opencv2/ximgproc.hpp>

#include "Cameras.hpp"
#include "DisparityMapCalculator.hpp"
#include "FakeCameras.hpp"
#include "header.hpp"
#include "TaskManager.hpp"
#include "Timer.hpp"

#define SHARED_MEMORY_NAME "MySharedMemory"
#define SHARED_MEMORY_SIZE 65536 //(1280*720*4)

TaskManager::TaskManager()
: numberOfSlaves_{4}
, numberOfTasks_{4}
, segment_{managed_shared_memory(open_or_create, SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE)}
{
    prepareSharedData();
}

void TaskManager::prepareSharedData()
{
}

void TaskManager::removeSegment()
{
    // when done, destroy the vector from the segment
    shared_memory_object::remove(SHARED_MEMORY_NAME);
}

void TaskManager::start()
{
    int pid, status;

    for (int slaveNumber = 0; slaveNumber < numberOfSlaves_; slaveNumber++)
    {
        if ((pid = fork()) == 0)
        {
            slaveTask(slaveNumber);
        }
    }

    masterTask();

    for (int slave = 0; slave < numberOfSlaves_; slave++)
    {
        pid = wait(&status);
        std::cout << "Slave " << pid << " ended his job and returned " << WEXITSTATUS(status) << std::endl;
    }

    removeSegment();
}

void TaskManager::masterTask()
{
    ImageCorrection ic;  // (calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    //Cameras cameras;
    FakeCameras cameras;
    Timer timer("Disparity");

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
        // divie into parts for each slave
        int width = mats.left.cols;
        int height = mats.left.rows;
        int partHeight = height / numberOfTasks_;

        cv::Mat map;

        std::vector<cv::Mat> subMaps;

        for (auto i = 0; i < numberOfTasks_; i++)
        {
            cv::Mat left = cv::Mat(mats.left, cv::Rect(0, i * partHeight, width, partHeight));
            cv::Mat right = cv::Mat(mats.right, cv::Rect(0, i * partHeight, width, partHeight));
            Mat subMap = dmc.getMap(left, right);
            subMaps.push_back(subMap);
        }

        // wait for map generation in slaves
        for (auto i = 0; i < numberOfTasks_; i++)
        {
            if (i > 0)
            {
                vconcat(map, subMaps[i], map);
            }
            else
            {
                map = subMaps[i];
            }
        }
        timer.measure(Timer::EMeasure::DISPARITY_MAP_GENERATED);
        // merge images
        // display images
        imshow("Cameras", Cameras::resizeAndConcat(mats.left, mats.right));
        imshow("Disparity", map);
        timer.measure(Timer::EMeasure::FRAME_END);
    } while(waitKey(30) != 27);
    timer.printLog();
}

void TaskManager::slaveTask(int number)
{
    DisparityMapCalculator dmc;

    auto active = true;

    while(active)
    {
        /*Mat subMap = dmc.getMap(leftSharedVector_[number], rightSharedVector_[number]);
        disparitySharedVector_[number] = subMap;*/
        active = false;
    }

    exit(0);
}