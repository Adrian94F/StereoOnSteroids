#include "Timer.hpp"

#include <numeric>
#include <set>

Timer::Timer(EMode mode)
    : mode_(mode)
{
    timesOfOperations_[EOperation::GETTING_IMAGES] = std::vector<double>();
    timesOfOperations_[EOperation::UNDISTORTION] = std::vector<double>();
    if (mode == EMode::SINGLETHREADED)
    {
        timesOfOperations_[EOperation::SINGLE_THREAD_DISPARITY_MAP_GENERATION] = std::vector<double>();
    }
    else
    {
        timesOfOperations_[EOperation::PREPARING_TASKS] = std::vector<double>();
        timesOfOperations_[EOperation::MULTI_THREAD_DISPARITY_MAP_GENERATION] = std::vector<double>();
        timesOfOperations_[EOperation::MERGING_RESULTS] = std::vector<double>();
    }
    timesOfOperations_[EOperation::DISPLAYING] = std::vector<double>();
}

void Timer::measure(EMeasure m)
{
    timestamps_.push_back({ getTime(), m });
}

void Timer::reset()
{
    timestamps_ = vector<timestamp>();
    startTime_ = getTime();
}

hrc::time_point Timer::getTime()
{
    return hrc::now();
}


void Timer::printLog()
{
    cout << std::fixed << setprecision(9);
    for (const auto& ts : timestamps_)
    {
        duration<double> timeSpan = duration_cast<duration<double>>(ts.time - startTime_);
        cout << timeSpan.count() << ": " << ts.measure << endl;
    }
}

void Timer::prepareStatistics()
{
    auto size = timestamps_.size();
    if (size < 2)
    {
        return;
    }
    for (auto t = 1; t < size; t++)
    {
        auto begin = timestamps_[t - 1];
        auto end = timestamps_[t];
        auto timeSpan = duration_cast<duration<double>>(end.time - begin.time).count();
        EOperation typeOfOperation;
        switch (end.measure)
        {
            case GOT_IMAGES:
                typeOfOperation = GETTING_IMAGES;
                break;
            case UNDISTORTED:
                typeOfOperation = UNDISTORTION;
                break;
            case DISPARITY_MAP_GENERATED:
                typeOfOperation = SINGLE_THREAD_DISPARITY_MAP_GENERATION;
                break;
            case PREPARED_TASKS:
                typeOfOperation = PREPARING_TASKS;
                break;
            case COMPLETED_TASKS:
                typeOfOperation = MULTI_THREAD_DISPARITY_MAP_GENERATION;
                break;
            case MERGED_RESULTS:
                typeOfOperation = MERGING_RESULTS;
                break;
            case DISPLAYED_RESULTS:
                typeOfOperation = DISPLAYING;
                break;
            default:
                typeOfOperation = CV_WAIT_KEY;
        }
        timesOfOperations_[typeOfOperation].push_back(timeSpan);
    }
}

void Timer::countMeanTimes()
{
    for (const auto& k : timesOfOperations_)
    {
        meanTimesOfOperations_[k.first] =
                std::accumulate(k.second.begin(), k.second.end(), 0.0) / double(k.second.size()) * 1000.0;
    }
}

void Timer::printStatistics(boost::optional<int> n)
{
    prepareStatistics();
    countMeanTimes();

    std::cout << "\n\nPrinting statistics for ";
    if (n)
    {
        std::cout << "multi-threaded version (" << n.value() << " slave" << (n.value() == 1 ? "" : "s") << ")\n";
    }
    else
    {
        std::cout <<"single-threaded version\n";
    }

    for (const auto& k : meanTimesOfOperations_)
    {
        switch (k.first)
        {
            case GETTING_IMAGES:
                std::cout << "Getting images:            ";
                break;
            case UNDISTORTION:
                std::cout << "Undistortion:              ";
                break;
            case SINGLE_THREAD_DISPARITY_MAP_GENERATION:
                std::cout << "Single-threaded disparity: ";
                break;
            case PREPARING_TASKS:
                std::cout << "Preparing tasks:           ";
                break;
            case MULTI_THREAD_DISPARITY_MAP_GENERATION:
                std::cout << "Multi-threaded disparity:  ";
                break;
            case MERGING_RESULTS:
                std::cout << "Merging results:           ";
                break;
            case DISPLAYING:
                std::cout << "Displaying:                ";
                break;
            case CV_WAIT_KEY:
                std::cout << "Waiting for key press:     ";
                break;
        }
        cout << std::fixed << setprecision(10);
        std::cout << setw(14) << k.second << " ms" << std::endl;
    }
}