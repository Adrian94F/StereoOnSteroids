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
std::vector<std::mutex> mutexTODO;
std::mutex mutexDone;
std::vector<std::condition_variable> tasksReadyToDo;
std::condition_variable taskDone;

void prepareVectors(int n)
{
    lInputs.resize(n);
    rInputs.resize(n);
    results.resize(n);
    threads.resize(n);
    tasksToDo.resize(n);
    mutexTODO = std::vector<std::mutex>(n);
    tasksReadyToDo = std::vector<std::condition_variable>(n);
}

void singleThreadProcessing(int nOfFrames = -1)
{
    FakeCameras cameras;
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    DisparityMapCalculator dmc;
    Timer timer(Timer::EMode::SINGLETHREADED);
    auto counter = 0;
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
    } while(waitKey(30) != 27 && (nOfFrames == -1 || counter++ < nOfFrames));
    timer.printStatistics();
}

void masterTask(int nOfTasks, int nOfFrames = -1)
{
    FakeCameras cameras;
    ImageCorrection ic(calibPath + calibFile);
    ImageCorrection::MatsPair mats;
    Timer timer(Timer::EMode::MULTITHREADED);

    DisparityMapCalculator dmc;

    auto counter = 0;
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
            tasksReadyToDo[i].notify_one();
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
    } while(waitKey(30) != 27 && (nOfFrames == -1 || counter++ < nOfFrames));

    // stop threads
    std::for_each(tasksToDo.begin(), tasksToDo.end(),
            [](auto& t)
            {
                t = {Task::ETaskStatus_Abort};
            });
    std::for_each(tasksReadyToDo.begin(), tasksReadyToDo.end(),
            [](auto& t)
            {
                t.notify_one();
            });


    // print statistics
    timer.printStatistics(nOfTasks);
}

void slaveTask(int id)
{
    DisparityMapCalculator dmc;

    auto active = true;

    while(active)
    {
        std::unique_lock<std::mutex> lk(mutexTODO[id]);
        tasksReadyToDo[id].wait(lk, [id](){
            return tasksToDo[id].status == Task::ETaskStatus_TODO || tasksToDo[id].status == Task::ETaskStatus_Abort;
        });

        if (tasksToDo[id].status == Task::ETaskStatus_Abort)
        {
            active = false;
        }

        // calculate disparity map
        results[id] = dmc.getMap(lInputs[id], rInputs[id]);

        tasksToDo[id].status = Task::ETaskStatus_Done;
        taskDone.notify_one();
    }
}

void multiThreadProcessing(int nOfTasks = 4, int nOfFrames = -1)
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

    masterTask(nOfTasks, nOfFrames);

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
        auto nOfFrames = 100;

        std::cout << "Running single-threaded\n";
        singleThreadProcessing(nOfFrames);

        std::vector<int> v(8);
        auto f = []()
                {
                    static int i = 1;
                    return i++;
                };
        std::generate(v.begin(), v.end(), f);
        v.push_back(16);
        v.push_back(32);
        std::for_each(v.begin(), v.end(), [nOfFrames](auto& n)
            {
                std::cout << "Running multi-threaded with " << n << (n == 1 ? " slave\n" : " slaves\n");
                multiThreadProcessing(n, nOfFrames);
            });

        return 0;
    }
}