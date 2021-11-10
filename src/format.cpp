#include <string>

#include "format.h"

using std::string;


string Format::ElapsedTime(long seconds) {
  unsigned long int hours;
  unsigned long int mins;
  unsigned long int secs;
  hours = seconds / 3600;
  mins = (seconds % 3600) / 60;
  secs = (seconds % 3600) % 60;
  string hours_str =hours < 10 ? "0" + std::to_string(hours) : std::to_string(hours);
  string mins_str = mins < 10 ? "0" + std::to_string(mins) : std::to_string(mins);
  string secs_str = secs < 10 ? "0" + std::to_string(secs) : std::to_string(secs);
  return string(hours_str+":"+mins_str+":"+secs_str);
}