#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>
struct CpuTime {
  unsigned long long int usertime = 0;
  unsigned long long int nicetime = 0;
  unsigned long long int systemtime = 0;
  unsigned long long int idletime = 0;
  unsigned long long int ioWait = 0;
  unsigned long long int irq = 0;
  unsigned long long int softIrq = 0;
  unsigned long long int steal = 0;
  unsigned long long int guest = 0;
  unsigned long long int guestnice = 0;
};

class Processor {
 public:
  float Utilization();

 private:
  CpuTime currentTime_, prevTime_;
  float Percentage();
  void GetCurrentCPUTime();
};

#endif