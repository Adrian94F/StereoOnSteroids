#ifndef CALIB_CAM_TIMER_HPP
#define CALIB_CAM_TIMER_HPP

#include <boost/optional.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <map>
#include <ratio>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;
using hrc = high_resolution_clock;

class Timer
        {
public:
    enum EMeasure
    {
        STARTED = 0,
        GOT_IMAGES = 1,
        UNDISTORTED = 2,
        DISPARITY_MAP_GENERATED = 3,
        PREPARED_TASKS = 4,
        COMPLETED_TASKS = 5,
        MERGED_RESULTS = 6,
        DISPLAYED_RESULTS = 7
    };
    enum EMode
    {
        SINGLETHREADED,
        MULTITHREADED
    };
    enum EOperation
    {
        GETTING_IMAGES = 0,
        UNDISTORTION,
        PREPARING_TASKS,
        MULTI_THREAD_DISPARITY_MAP_GENERATION,
        MERGING_RESULTS,
        DISPARITY_MAP_GENERATION,
        DISPLAYING,
        CV_WAIT_KEY,
        SUM
    };
    Timer() = default;
    explicit Timer(EMode);
    void measure(EMeasure m);
    void reset();
    void printLog();
    void printStatistics(boost::optional<int> n = boost::none);
private:
    hrc::time_point getTime();
    void prepareStatistics();
    void countMeanTimes();
    EMode mode_;
    struct Timestamp{
        hrc::time_point time;
        EMeasure measure;
    };
    std::vector <Timestamp> timestamps_;
    hrc::time_point startTime_;
    std::map<EOperation, std::vector<double>> timesOfOperations_;
    std::map<EOperation, double> meanTimesOfOperations_;
};


#endif //CALIB_CAM_TIMER_HPP
