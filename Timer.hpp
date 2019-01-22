#ifndef CALIB_CAM_TIMER_HPP
#define CALIB_CAM_TIMER_HPP

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <ratio>
#include <string>
#include <vector>

using namespace std;
using namespace std::chrono;
using hrc = high_resolution_clock;

class Timer {
public:
    Timer() = default;
    explicit Timer(string name="");
    void measure(string label="");
    void reset();
    void printLog();
private:
    hrc::time_point getTime();
    string name_ = "";
    struct {
        hrc::time_point time;
        string text;
    } typedef timestamp;
    vector <timestamp> timestamps_;
    hrc::time_point startTime_;
};


#endif //CALIB_CAM_TIMER_HPP
