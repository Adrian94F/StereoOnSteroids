#include "TaskManager.hpp"

#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <unistd.h>

#include <opencv2/ximgproc.hpp>

#include "Cameras.hpp"
#include "DisparityMapCalculator.hpp"
#include "header.hpp"
#include "TaskManager.hpp"
#include "Timer.hpp"

#define SHARED_MEMORY_NAME "MySharedMemory45==80893763"
#define SHARED_MEMORY_SIZE 65536 //(1280*720*4)
#define LEFT_INPUT_VECTOR "left_vec"
#define RIGHT_INPUT_VECTOR "right_vec"
#define DISPARITY_MAP_VECTOR "disparity_vec"
#define TASK_READY_MUTEX_VECTOR "task_ready_mutex_vector"
#define TASK_DONE_MUTEX_VECTOR "task_done_mutex_vector"

TaskManager::TaskManager()
: numberOfSlaves_{4}
, numberOfTasks_{4}
, segment_{managed_shared_memory(open_or_create, SHARED_MEMORY_NAME, SHARED_MEMORY_SIZE)}
, mat_alloc_inst_{segment_.get_segment_manager()}
, mutex_alloc_inst_{segment_.get_segment_manager()}
, leftSharedVector_{*segment_.construct<SharedMatVector>(LEFT_INPUT_VECTOR)(mat_alloc_inst_)}
, rightSharedVector_{*segment_.construct<SharedMatVector>(RIGHT_INPUT_VECTOR)(mat_alloc_inst_)}
, disparitySharedVector_{*segment_.construct<SharedMatVector>(DISPARITY_MAP_VECTOR)(mat_alloc_inst_)}
, taskReadyMutexSharedVector_{*segment_.construct<SharedMutexVector>(TASK_READY_MUTEX_VECTOR)(mutex_alloc_inst_)}
, taskDoneMutexSharedVector_{*segment_.construct<SharedMutexVector>(TASK_DONE_MUTEX_VECTOR)(mutex_alloc_inst_)}
{
    prepareSharedData();
}

void TaskManager::prepareSharedData()
{
    // resize vectors
    leftSharedVector_.resize(numberOfTasks_);
    rightSharedVector_.resize(numberOfTasks_);
    disparitySharedVector_.resize(numberOfTasks_);
    taskReadyMutexSharedVector_.resize(numberOfTasks_);
    taskDoneMutexSharedVector_.resize(numberOfTasks_);

    for (auto i = 0; i < numberOfTasks_; i++)
    {
        taskDoneMutexSharedVector_[i] = new boost::mutex();
        taskReadyMutexSharedVector_[i] = new boost::mutex();
        taskReadyMutexSharedVector_[i]->lock();
    }
}

void TaskManager::removeSegment()
{
    // wen done, destroy the vector from the segment
    segment_.destroy<SharedMatVector>(LEFT_INPUT_VECTOR);
    segment_.destroy<SharedMatVector>(RIGHT_INPUT_VECTOR);
    segment_.destroy<SharedMatVector>(DISPARITY_MAP_VECTOR);
    segment_.destroy<SharedMutexVector>(TASK_READY_MUTEX_VECTOR);
    segment_.destroy<SharedMutexVector>(TASK_DONE_MUTEX_VECTOR);
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
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Cameras cameras;
    Timer timer("Disparity");
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

        leftSharedVector_.clear();
        rightSharedVector_.clear();

        disparitySharedVector_.clear();
        cv::Mat map;

        for (auto i = 0; i < numberOfTasks_; i++)
        {
            cv::Mat left = cv::Mat(mats.left, cv::Rect(0, i * partHeight, width, partHeight));
            cv::Mat right = cv::Mat(mats.right, cv::Rect(0, i * partHeight, width, partHeight));
            leftSharedVector_[i] = left;
            rightSharedVector_[i] = right;
            taskReadyMutexSharedVector_[i]->unlock();
        }

        // wait for map generation in slaves
        for (auto i = 0; i < numberOfTasks_; i++)
        {
            taskDoneMutexSharedVector_[i]->lock();
            if (i > 0)
            {
                vconcat(map, disparitySharedVector_[i], map);
            }
            else
            {
                map = disparitySharedVector_[i];
            }
            taskDoneMutexSharedVector_[i]->unlock();

            taskReadyMutexSharedVector_[i]->lock();
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

        taskReadyMutexSharedVector_[number]->lock();

        taskDoneMutexSharedVector_[number]->lock();

        Mat subMap = dmc.getMap(leftSharedVector_[number], rightSharedVector_[number]);
        disparitySharedVector_[number] = subMap;

        taskDoneMutexSharedVector_[number]->unlock();

        taskReadyMutexSharedVector_[number]->unlock();
        active = false;
    }

    exit(0);
}