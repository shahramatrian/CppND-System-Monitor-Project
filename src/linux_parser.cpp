#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

template <typename T>
T findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line;
  std::string key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          stream.close();
          return value;
        }
      }
    }
    stream.close();
  }
  return value;
};

template <typename T>
T getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
    stream.close();
  }
  return value;
};

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value = "Linux"; // if not set
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
          filestream.close();
          return value;
        }
      }
    }
    filestream.close();
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
    stream.close();
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
  string keyMemTotal = "MemTotal:";
  string keyMemFree = "MemFree:";
  float totalMemory = findValueByKey<float>(keyMemTotal, kMeminfoFilename);
  float freeMemory = findValueByKey<float>(keyMemFree, kMeminfoFilename);

  return (totalMemory - freeMemory) / totalMemory;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  return getValueOfFile<long>(kUptimeFilename);
}

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string key, line;
  vector<string> currentStat;
  std::string usertime;
  std::string nicetime;
  std::string systemtime;
  std::string idletime;
  std::string ioWait;
  std::string irq;
  std::string softIrq;
  std::string steal;
  std::string guest;
  std::string guestnice;
  std::ifstream statStream(LinuxParser::kProcDirectory +
                           LinuxParser::kStatFilename);

  if (statStream.is_open()) {
    while (std::getline(statStream, line)) {
      std::istringstream lineStream(line);
      while (lineStream >> key) {
        if (key == "cpu") {
          lineStream >> usertime >> nicetime >> systemtime >> idletime >>
              ioWait >> irq >> softIrq >> steal >> guest >> guestnice;

          currentStat = {usertime, nicetime ,systemtime , idletime , ioWait , irq , softIrq , steal , guest , guestnice};
          statStream.close();
          return currentStat;
        }
      }
    }
    statStream.close();
  }
  return currentStat;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string keyProcesses = "processes";
  return findValueByKey<int>(keyProcesses, kStatFilename);
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string key = "procs_running";
  return findValueByKey<int>(key, kStatFilename);
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
    stream.close();
    return line;
  }
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string ram = "0";
  string vmSize = "VmRSS:"; // I used VmRSS instead of VmSize because VmSize is the sum of all virtual memory

  ram = findValueByKey<string>(vmSize, std::to_string(pid) + kStatusFilename);
  ram = (ram.empty() || ram.length() < 3) ? "0" : 
        ram.substr(0, ram.length() -3); // deleting the last three characters instead of std::to_string(std::stol(ram) / 1024);

  return ram;
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string key = "Uid:";
  return findValueByKey<string>(key, std::to_string(pid) + LinuxParser::kStatusFilename);
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string uname;
  string xchar;
  string uId;
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
        stream.close();
        return userName;
      }
    }
    stream.close();
  }
  return userName;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  long int uptime{0};

  const unsigned long Hertz = sysconf(_SC_CLK_TCK);  // system clock ticks/second
  string line;
  string token;
  
  std::ifstream stream(LinuxParser::kProcDirectory + std::to_string(pid) +
                       LinuxParser::kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream lineStream(line);
    for (int i = 0; i < 22; ++i) {
      lineStream >> token;
      if (i == 21) {
          uptime = UpTime() - stol(token) / Hertz; //token is the process starttime which is 22nd token
          stream.close();
          return uptime;
      }
    }
    stream.close();
  }
  return uptime;
}
