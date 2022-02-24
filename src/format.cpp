#include "format.h"

#include <iomanip>
#include <sstream>
#include <string>

using std::string;
using std::to_string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::Time(long int second) {
  tm timeinfo;

  timeinfo.tm_sec = second % 60;
  timeinfo.tm_min = (second % 3600) / 60;
  timeinfo.tm_hour = second / 3600;
  std::stringstream buffer;
  buffer << std::put_time(&timeinfo, "%T");
  return buffer.str();

  /* Other ways to do it:
  // with sprintf
  int hours = second / 3600;
  int minutes = (second % 3600) / 60;
  int seconds = second % 60;

  char *time_str = new char[80];
  sprintf(time_str, "%02i:%02i:%02i", hours, minutes, seconds);

  // OR with strftime
  strftime (time_str,80,"%T.", &timeinfo);

  return time_str;*/
}