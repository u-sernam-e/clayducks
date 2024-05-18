#include "timer.h"

long long getSystemTimeMil()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

long long getMilTimeSince(long long m)
{
    return getSystemTimeMil() - m;
}