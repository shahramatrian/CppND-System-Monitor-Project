#include "process.h"

#include <unistd.h>

#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// Construct a Process object
Process::Process(int pid) { this->pid = pid; }

// Return this process's ID
int Process::Pid() { return pid; }

// Return this process's CPU utilization
/**
 * @brief Return this process's CPU utilization based on:
 * https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
 *
 * @return float
 */
float Process::CpuUtilization() {
  cpu_usage = 0.0;
  const unsigned long Hertz =
      sysconf(_SC_CLK_TCK);  // system clock ticks/second

  string line;
  long utime;   // token #14 - CPU time spent in user code, measured in clock
                // ticks
  long stime;   // token #15 - CPU time spent in kernel code, measured in clock
                // ticks
  long cutime;  // token #16 - Waited-for children's CPU time spent in user code
                // (in clock ticks)
  long cstime;  // token #17 - Waited-for children's CPU time spent in kernel
                // code (in clock ticks)
  long starttime;  // token #22 - Time when the process started, measured in
                   // clock ticks

  std::string tokens[22];
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);
    for (int i = 0; i < 22; ++i) {
      lineStream >> tokens[i];
    }

    utime = std::stoi(tokens[13]);
    stime = std::stoi(tokens[14]);
    cutime = std::stoi(tokens[15]);
    cstime = std::stoi(tokens[16]);
    starttime = std::stoi(tokens[21]);

    long total_time = utime + stime + cutime +
                      cstime;  // total time spent for the process including the
                               // time from children processes
    unsigned long seconds =
        LinuxParser::UpTime() -
        (starttime /
         Hertz);  // total elapsed time in seconds since the process started
    if (seconds > 0) {
      cpu_usage = (((float)total_time / (float)Hertz) / (float)seconds);
    }
  }

  return cpu_usage;
}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// Return this process's memory utilization
string Process::Ram() {
  return LinuxParser::Ram(pid);
}

// Return the user (name) that generated this process
string Process::User() {
  if (userName.empty()) {
    userName = LinuxParser::User(pid);
  }
  return userName;
}

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(const Process& a) const {
  return cpu_usage < a.cpu_usage;
}

bool Process::operator>(const Process& a) const {
  return cpu_usage > a.cpu_usage;
}