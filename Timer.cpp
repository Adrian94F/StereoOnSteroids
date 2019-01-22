#include "Timer.hpp"

Timer::Timer(string name): name_(move(name))
{}

void Timer::measure(string label)
{
    timestamps_.push_back({ getTime(), move(label) });
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
    cout << std::fixed << setprecision(9) << name_ << endl;
    for (const auto& ts : timestamps_)
    {
        duration<double> timeSpan = duration_cast<duration<double>>(ts.time - startTime_);
        cout << timeSpan.count() << ": " << ts.text << endl;
    }
}