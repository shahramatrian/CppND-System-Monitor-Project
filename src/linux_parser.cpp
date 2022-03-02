#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float utilizationPercent = 0.0;
  string key, line;
  float totalMemory, value, freeMemory;

  std::ifstream memStream(kProcDirectory + kMeminfoFilename);
  if (memStream.is_open()) {
    while (std::getline(memStream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "MemTotal:") {
        totalMemory = value;
      }
      if (key == "MemFree:") {
        freeMemory = value;
      }
    }
  }
  utilizationPercent = (totalMemory - freeMemory) / totalMemory;
  return utilizationPercent;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string value;

  std::ifstream utStream(kProcDirectory + kUptimeFilename);
  if (utStream.is_open()) {
    std::getline(utStream, line);
    std::istringstream lineStream(line);
    lineStream >> value;
  }
  utStream.close();
  return std::stol(value);
}

// (Unnecessary) Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// (Unnecessary) Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid [[maybe_unused]]) { return 0; }

// (Unnecessary) Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// (Unnecessary) Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string key, line;
  vector<string> currentStat;
  string usertime, nicetime, systemtime, idletime, ioWait, irq, softIrq, steal,
      guest, guestnice;
  std::ifstream statStream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);

  if (statStream.is_open()) {
    while (std::getline(statStream, line)) {
      std::istringstream lineStream(line);
      while (lineStream >> key) {
        if (key == "cpu") {
          lineStream >> usertime >> nicetime >> systemtime >> idletime >>
              ioWait >> irq >> softIrq >> steal >> guest >> guestnice;

          currentStat.push_back(usertime);
          currentStat.push_back(nicetime);
          currentStat.push_back(systemtime);
          currentStat.push_back(idletime);
          currentStat.push_back(ioWait);
          currentStat.push_back(irq);
          currentStat.push_back(softIrq);
          currentStat.push_back(steal);
          currentStat.push_back(guest);
          currentStat.push_back(guestnice);
          return currentStat;
        }
      }
    }
  }
  return currentStat;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string key, line;
  int value;

  std::ifstream statStream(kProcDirectory + kStatFilename);
  if (statStream.is_open()) {
    while (std::getline(statStream, line)) {
      std::istringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == "processes") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key, line;
  int value;

  std::ifstream statStream(kProcDirectory + kStatFilename);
  if (statStream.is_open()) {
    while (std::getline(statStream, line)) {
      std::istringstream lineStream(line);
      while (lineStream >> key >> value) {
        if (key == "procs_running") {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command;
  string line;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);

    command = line;
    return command;
  }
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string ram = "0";
  string line, key;
  string value;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "VmSize:") {
        ram = value.empty() ? "0" : std::to_string(std::stol(value) / 1024);
        return ram;
      }
    }
  }

  return ram;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line, key, userId;
  string value;

  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream lineStream(line);
      lineStream >> key >> value;
      if (key == "Uid:") {
        userId = value;
        return userId;
      }
    }
  }
  return userId;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line, uname, xchar, uId;
  string userName;

  string userId = LinuxParser::Uid(pid);

  std::ifstream stream(LinuxParser::kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream lineStream(line);
      lineStream >> uname >> xchar >> uId;
      if (uId == userId) {
        userName = uname;
        return userName;
      }
    }
  }
  return userName;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long int uptime{0};
  // long starttime = 0; // token #22 - Time when the process started, measured in clock ticks
  // long utime = 0; // token #14 - CPU time spent in user mode, measured in clock ticks
  const unsigned long Hertz =
      sysconf(_SC_CLK_TCK);  // system clock ticks/second
  string line;

  std::string tokens[22];
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);
    for (int i = 0; i < 22; ++i) {
      lineStream >> tokens[i];
    }

    uptime = UpTime() - stol(tokens[21]) / Hertz;
    stream.close();
  }
  return uptime;
}
