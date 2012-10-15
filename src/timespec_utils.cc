#ifndef __TIMESPEC_UTILS_HH__
#define __TIMESPEC_UTILS_HH__

#include <ctime>

namespace timespec_utils {

timespec realtime_now()
{
    timespec out;
    clock_gettime(CLOCK_REALTIME, &out);
    return out;
}

double to_double(const timespec & ts)
{
    return ts.tv_sec + ((double)ts.tv_nsec / 1.0e9);
}

timespec ts_diff(const timespec & end, const timespec & start)
{
    timespec out;
    if((end.tv_nsec - start.tv_nsec) < 0)
    {
        out.tv_sec = end.tv_sec - start.tv_sec - 1;
        out.tv_nsec = 1e9 + end.tv_nsec - start.tv_nsec;
    }
    else
    {
        out.tv_sec = end.tv_sec - start.tv_sec;
        out.tv_nsec = end.tv_nsec - start.tv_nsec;
    }
    return out;
}

} // end namespace timespec_utils

#endif

