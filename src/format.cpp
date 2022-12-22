#include <iomanip>
#include <string>

#include "format.h"

using std::string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long input_seconds) {
  auto hours = std::to_string((input_seconds % Format::seconds_in_a_day) /
                              Format::seconds_in_a_hour);
  auto minutes =
      std::to_string(((input_seconds % Format::seconds_in_a_day)) %
                     Format::seconds_in_a_hour / Format::seconds_in_a_minute);
  auto seconds = std::to_string(
      ((input_seconds % Format::seconds_in_a_day) % Format::seconds_in_a_hour) %
      Format::seconds_in_a_minute);

  std::ostringstream stream;

  stream << std::setw(2) << std::setfill('0') << hours << ":" << std::setw(2)
         << std::setfill('0') << minutes << ":" << std::setw(2)
         << std::setfill('0') << seconds;
  return stream.str();
}