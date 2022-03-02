#include <chrono>
#include <iostream>

class TimeClick 
{
private:
    std::chrono::time_point<std::chrono::system_clock> start;
    std::chrono::time_point<std::chrono::system_clock> end;
    bool isStarted;
    bool isStopped;

public:
    TimeClick();
    ~TimeClick();
    bool startClick();
    bool stopClick();
    void resetClick();
    int64_t getElapsedTime() const;
    int64_t getElapsedTimeinHours() const;
    int64_t getElapsedTimeinMinutes() const;
    int64_t getElapsedTimeinSeconds() const;
    int64_t getElapsedTimeinMilliseconds() const;
    int64_t getElapsedTimeinMicroseconds() const;
    int64_t getElapsedTimeinNanoseconds() const;
    void printElapsedTime(bool print_seconds=false, bool print_minutes=false, bool print_hours=false) const;
};

