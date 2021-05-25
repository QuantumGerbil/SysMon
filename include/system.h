#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>
#include <experimental/optional>

#include "process.h"
#include "processor.h"

class System {
 public:
  Processor& Cpu();                   // TODO: See src/system.cpp
  std::vector<Process>& Processes();  // TODO: See src/system.cpp
  float MemoryUtilization();          // TODO: See src/system.cpp
  long UpTime();                      // TODO: See src/system.cpp
  int TotalProcesses();               // TODO: See src/system.cpp
  int RunningProcesses();             // TODO: See src/system.cpp
  std::string Kernel();               // TODO: See src/system.cpp
  std::string OperatingSystem();      // TODO: See src/system.cpp

  // TODO: Define any necessary private members
 private:
  std::experimental::optional<Processor> cpu_;
  std::vector<Process> processes_;
  std::experimental::optional<std::string> kernel_;
  std::experimental::optional<std::pair<long,float> > memUtilization_;
  long int upTime_{0};
  std::experimental::optional<std::string> os_;
  std::experimental::optional<std::pair<long, long> > runningProcCount_;
  std::experimental::optional<std::pair<long, long> > totalProcCount_;
  
};

#endif