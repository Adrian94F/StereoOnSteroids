#include "Timer.hpp"

#include <numeric>
#include <set>

Timer::Timer(EMode mode)
    : mode_(mode)
{
    timesOfOperations_[EOperation::GETTING_IMAGES] = std::vector<double>();
    timesOfOperations_[EOperation::UNDISTORTION] = std::vector<double>();
    if (mode == EMode::MULTITHREADED)
    {
        timesOfOperations_[EOperation::PREPARING_TASKS] = std::vector<double>();
        timesOfOperations_[EOperation::MULTI_THREAD_DISPARITY_MAP_GENERATION] = std::vector<double>();
        timesOfOperations_[EOperation::MERGING_RESULTS] = std::vector<double>();
    }
    timesOfOperations_[EOperation::DISPARITY_MAP_GENERATION] = std::vector<double>();
    timesOfOperations_[EOperation::DISPLAYING] = std::vector<double>();
}

void Timer::measure(EMeasure m)
{
    timestamps_.push_back({ getTime(), m });
}

void Timer::reset()
{
    timestamps_ = vector<Timestamp>();
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
                typeOfOperation = DISPARITY_MAP_GENERATION;
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
        if (end.measure == MERGED_RESULTS)
        {
            // sum preparing tasks, map generation and merging results
            begin = timestamps_[t - 3];
            timeSpan = duration_cast<duration<double>>(end.time - begin.time).count();
            typeOfOperation = DISPARITY_MAP_GENERATION;
            timesOfOperations_[typeOfOperation].push_back(timeSpan);
        }
    }
}

void Timer::countMeanTimes()
{
    meanTimesOfOperations_[EOperation::SUM] = 0;
    for (const auto& k : timesOfOperations_)
    {
        meanTimesOfOperations_[k.first] =
                std::accumulate(k.second.begin(), k.second.end(), 0.0) / double(k.second.size()) * 1000.0;
        if (k.first != EOperation::PREPARING_TASKS &&
            k.first != EOperation::MULTI_THREAD_DISPARITY_MAP_GENERATION &&
            k.first != EOperation::MERGING_RESULTS)
        {
            meanTimesOfOperations_[EOperation::SUM] += meanTimesOfOperations_[k.first];
        }
    }
}

void Timer::printStatistics(boost::optional<int> n)
{
    prepareStatistics();
    countMeanTimes();

    std::cout << "Printing statistics for ";
    if (n)
    {
        std::cout << "multi-threaded version (" << n.value() << " slave" << (n.value() == 1 ? "" : "s") << ")\n";
    }
    else
    {
        std::cout <<"single-threaded version\n";
    }

    auto sum = meanTimesOfOperations_[EOperation::SUM];

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
            case DISPARITY_MAP_GENERATION:
                std::cout << "Disparity:                 ";
                break;
            case PREPARING_TASKS:
                std::cout << "  Preparing tasks:         ";
                break;
            case MULTI_THREAD_DISPARITY_MAP_GENERATION:
                std::cout << "  Partial disparity:       ";
                break;
            case MERGING_RESULTS:
                std::cout << "  Merging results:         ";
                break;
            case DISPLAYING:
                std::cout << "Displaying:                ";
                break;
            case CV_WAIT_KEY:
                std::cout << "Waiting for key press:     ";
                break;
            case SUM:
                std::cout << "Sum:                       ";
                break;
        }
        auto value = k.second;
        auto percent = value / sum * 100;
        cout << std::fixed << setprecision(3) << setw(7) << value << " ms (";
        cout << std::fixed << setprecision(2) << setw(6) << percent << "%)\n";
    }
    std::cout << std::endl;
}