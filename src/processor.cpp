#include "processor.h"

#include <string>
#include <vector>

#include "linux_parser.h"

using namespace std;
using std::string;

// Return the aggregate CPU utilization
float Processor::Utilization() {
  GetCurrentCPUTime();
  float CPU_Percentage = Percentage();

  this->prevTime_ = this->currentTime_;
  return CPU_Percentage;
}

/**
 * @brief Calculates CPU usage percentage based on algorithm suggested by:
 * https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
 *
 * @return float
 */

float Processor::Percentage() {
  float CPU_Percentage = 0.0;
  unsigned long long int prevIdletime = prevTime_.idletime + prevTime_.ioWait;
  unsigned long long int idletime = currentTime_.idletime + currentTime_.ioWait;

  unsigned long long int prevNonIdleTime =
      prevTime_.usertime + prevTime_.nicetime + prevTime_.systemtime +
      prevTime_.irq + prevTime_.softIrq + prevTime_.steal;

  unsigned long long int nonIdleTime =
      currentTime_.usertime + currentTime_.nicetime + currentTime_.systemtime +
      currentTime_.irq + currentTime_.softIrq + currentTime_.steal;

  unsigned long long int prevTotal = prevIdletime + prevNonIdleTime;
  unsigned long long int total = idletime + nonIdleTime;

  // differentiate: actual value minus the previous one
  float totald = total - prevTotal;
  float idled = idletime - prevIdletime;

  if (totald > 0) {
    CPU_Percentage = (totald - idled) / totald;
  }
  return CPU_Percentage;
}

/**
 * @brief Extracts CPU usage time from /proc/stat
 *
 */
void Processor::GetCurrentCPUTime() {
  vector<string> utilization = LinuxParser::CpuUtilization();
  if (utilization.size() > 0) {
    currentTime_.usertime = stoi(utilization.at(0));
    currentTime_.nicetime = stoi(utilization.at(1));
    currentTime_.systemtime = stoi(utilization.at(2));
    currentTime_.idletime = stoi(utilization.at(3));
    currentTime_.ioWait = stoi(utilization.at(4));
    currentTime_.irq = stoi(utilization.at(5));
    currentTime_.softIrq = stoi(utilization.at(6));
    currentTime_.steal = stoi(utilization.at(7));
    currentTime_.guest = stoi(utilization.at(8));
    currentTime_.guestnice = stoi(utilization.at(9));
  }
}