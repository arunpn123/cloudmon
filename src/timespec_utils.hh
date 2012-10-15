#ifndef __TIMESPEC_UTILS_HH__
#define __TIMESPEC_UTILS_HH__

#include <ctime>

namespace timespec_utils {

timespec realtime_now();
double to_double(const timespec & ts);
timespec ts_diff(const timespec & end, const timespec & start);

} // end namespace timespec_utils

#endif

