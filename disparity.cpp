#include <condition_variable>
#include <mutex>
#include <opencv2/ximgproc.hpp>
#include <thread>

#include "Cameras.hpp"
#include "DisparityMapCalculator.hpp"
#include "FakeCameras.hpp"
#include "header.hpp"
#include "ImageCorrection.hpp"
#include "settings.hpp"
#include "Task.hpp"
#include "Timer.hpp"

using namespace cv::ximgproc;

std::vector<cv::Mat> lInputs;
std::vector<cv::Mat> rInputs;
std::vector<cv::Mat> results;
std::vector<std::thread> threads;
std::vector<Task> tasksToDo;
std::mutex mutexTODO;
std::mutex mutexDone;
std::condition_variable taskTODO;
std::condition_variable taskDone;

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
            auto offsetTop = i == 0 ? 0 : BORDER;
            auto offsetBottom = i != (tasksToDo.size() -1) ? BORDER : 0;
            auto y0 = i * height - offsetTop;
            auto y = height + offsetTop + offsetBottom;
            lInputs[i] = cv::Mat(mats.left, cv::Rect(0, y0, width, y));
            rInputs[i] = cv::Mat(mats.right, cv::Rect(0, y0, width, y));

            // Task i ready to process
            tasksToDo[i] = {Task::ETaskStatus_TODO};
            taskTODO.notify_all();
            //std::cout << "sent notif to " << i << '\n';
        }
        timer.measure(Timer::EMeasure::PREPARED_TASKS);

        // wait for disparityMap generation in slaves
        std::unique_lock<std::mutex> lk(mutexDone);
        taskDone.wait(lk, [nOfTasks]()
            {
                return std::count_if(
                        tasksToDo.begin(), tasksToDo.end(),
                        [](auto task){ return task.status == Task::ETaskStatus_Done; }
                    ) == nOfTasks;
            });

        timer.measure(Timer::EMeasure::COMPLETED_TASKS);

        // merge images
        for (auto i = 0; i < results.size(); i++)
        {
            auto submap = cv::Mat(results[i], cv::Rect(0, BORDER, results[i].cols, results[i].rows - 2 * BORDER));
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
            [](auto& t)
            {
                t = {Task::ETaskStatus_Abort};
            });
    taskTODO.notify_all();


    // print statistics
    timer.printStatistics(nOfTasks);
}

void slaveTask(int id)
{
    DisparityMapCalculator dmc;

    // wait for master readiness
    while(tasksToDo[id].status == Task::ETaskStatus_Wait);
    //while(tasksToDo[id].status != Task::ETaskStatus_Abort)

    auto active = true;

    //std::cout << id << " ready\n";
    while(active)
    {
        std::unique_lock<std::mutex> lk(mutexTODO);
        taskTODO.wait(lk, [id](){
            return tasksToDo[id].status == Task::ETaskStatus_TODO || tasksToDo[id].status == Task::ETaskStatus_Abort;
        });

        //std::cout << id << " got notif\n";

        //while(tasksToDo[id].status != Task::ETaskStatus_TODO && tasksToDo[id].status != Task::ETaskStatus_Abort);
        if (tasksToDo[id].status == Task::ETaskStatus_Abort)
        {
            active = false;
        }

        // calculate disparity map
        results[id] = dmc.getMap(lInputs[id], rInputs[id]);

        tasksToDo[id].status = Task::ETaskStatus_Done;
        taskDone.notify_all();
    }
}

void multiThreadProcessing(int nOfTasks = 4)
{
    prepareVectors(0);
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
    if (argc == 2)
    {
        auto num = strtol(argv[1], nullptr, 10);
        if (num < 0)
        {
            std::cerr << "wrong argument" << std::endl;
            return -1;
        }
        else
        {
            if (num == 0)
            {
                std::cout << "Running single threaded version" << std::endl;
                singleThreadProcessing();
            }
            else
            {
                std::cout << "Running multi-threaded version with " << num << " workers\n";
                multiThreadProcessing(num);
            }
        }
    }
    else
    {
        std::cout << "Running single-threaded\n";
        singleThreadProcessing();
        uint n = 1;
        do
        {
            std::cout << "Running multi-threaded with " << n << (n == 1 ? "slave\n" : " slaves\n");
            multiThreadProcessing(n);
            n *= 2;
        } while (n <= 16);
        return 0;
    }
}