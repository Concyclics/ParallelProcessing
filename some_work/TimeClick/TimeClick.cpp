#include "TimeClick.h"

TimeClick::TimeClick()
{
    resetClick();
}

TimeClick::~TimeClick()
{
}

bool TimeClick::startClick()
{
    if (isStarted)
    {
        std::cout<<"TimeClick: startClick() called while already started"<<std::endl;
        return false;
    }
    start = std::chrono::system_clock::now();
    isStarted = true;
    return true;
}

bool TimeClick::stopClick()
{
    if (!isStarted)
    {
        std::cout<<"TimeClick: stopClick() called while not started"<<std::endl;
        return false;
    }
    if (isStopped)
    {
        std::cout<<"TimeClick: stopClick() called while already stopped"<<std::endl;
        return false;
    }
    end = std::chrono::system_clock::now();
    isStopped = true;
    return true;
}

void TimeClick::resetClick()
{
    isStarted = false;
    isStopped = false;
}

int64_t TimeClick::getElapsedTime() const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: getElapsedTime() called while not stopped"<<std::endl;
        return -1;
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
}

int64_t TimeClick::getElapsedTimeinHours() const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: getElapsedTimeinHours() called while not stopped"<<std::endl;
        return -1;
    }
    return std::chrono::duration_cast<std::chrono::hours>(end-start).count();
}

int64_t TimeClick::getElapsedTimeinMinutes() const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: getElapsedTimeinMinutes() called while not stopped"<<std::endl;
        return -1;
    }
    return std::chrono::duration_cast<std::chrono::minutes>(end-start).count();
}

int64_t TimeClick::getElapsedTimeinSeconds() const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: getElapsedTimeinSeconds() called while not stopped"<<std::endl;
        return -1;
    }
    return std::chrono::duration_cast<std::chrono::seconds>(end-start).count();
}

int64_t TimeClick::getElapsedTimeinMilliseconds() const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: getElapsedTimeinMilliseconds() called while not stopped"<<std::endl;
        return -1;
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(end-start).count();
}

int64_t TimeClick::getElapsedTimeinMicroseconds() const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: getElapsedTimeinMicroseconds() called while not stopped"<<std::endl;
        return -1;
    }
    return std::chrono::duration_cast<std::chrono::microseconds>(end-start).count();
}

int64_t TimeClick::getElapsedTimeinNanoseconds() const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: getElapsedTimeinNanoseconds() called while not stopped"<<std::endl;
        return -1;
    }
    return std::chrono::duration_cast<std::chrono::nanoseconds>(end-start).count();
} 

void TimeClick::printElapsedTime(bool print_seconds, bool print_minutes, bool print_hours) const
{
    if (!isStopped)
    {
        std::cout<<"TimeClick: printElapsedTime() called while not stopped"<<std::endl;
        return;
    }
    int64_t elapsed_time = getElapsedTime();
    int64_t elapsed_time_in_hours = getElapsedTimeinHours();
    int64_t elapsed_time_in_minutes = getElapsedTimeinMinutes();
    int64_t elapsed_time_in_seconds = getElapsedTimeinSeconds();
    int64_t elapsed_time_in_milliseconds = getElapsedTimeinMilliseconds();
    int64_t elapsed_time_in_microseconds = getElapsedTimeinMicroseconds();
    int64_t elapsed_time_in_nanoseconds = getElapsedTimeinNanoseconds();
    std::cout<<"TimeClick: elapsed time: ";
    if (print_hours)
    {
        std::cout<<elapsed_time_in_hours<<" hours, ";
        elapsed_time_in_minutes -= elapsed_time_in_hours*60;
        elapsed_time_in_seconds -= elapsed_time_in_hours*3600;
        elapsed_time_in_milliseconds -= elapsed_time_in_hours*3600000;
        elapsed_time_in_microseconds -= elapsed_time_in_hours*3600000000;
        elapsed_time_in_nanoseconds -= elapsed_time_in_hours*3600000000000;
    }
    if (print_minutes)
    {
        std::cout<<elapsed_time_in_minutes<<" minutes, ";
        elapsed_time_in_seconds -= elapsed_time_in_minutes*60;
        elapsed_time_in_milliseconds -= elapsed_time_in_minutes*60000;
        elapsed_time_in_microseconds -= elapsed_time_in_minutes*60000000;
        elapsed_time_in_nanoseconds -= elapsed_time_in_minutes*60000000000;
    }
    if (print_seconds)
    {
        std::cout<<elapsed_time_in_seconds<<" seconds, ";
        elapsed_time_in_milliseconds -= elapsed_time_in_seconds*1000;
        elapsed_time_in_microseconds -= elapsed_time_in_seconds*1000000;
        elapsed_time_in_nanoseconds -= elapsed_time_in_seconds*1000000000;
    }
    std::cout<<elapsed_time_in_milliseconds<<" milliseconds. "<<std::endl;
}