#include "format.h"

#include <string>

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::Time(long int second) {
  int hours = second / 3600;
  int minutes = (second / 60) % 60;
  int seconds = second % 60;

  char *time_str = new char[20];
  sprintf(time_str, "%02i:%02i:%02i ", hours, minutes, seconds);

  return time_str;
}