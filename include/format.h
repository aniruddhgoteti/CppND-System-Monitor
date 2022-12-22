#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);
long const seconds_in_a_day = 86400;
long const seconds_in_a_hour = 3600;
long const seconds_in_a_minute = 60;
};  // namespace Format

#endif