#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <thread>
#include "linux_parser.h"

#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Done: Return the system's CPU
Processor& System::Cpu() {
  if(!cpu_){
    cpu_ = Processor(LinuxParser::ActiveJiffies(), LinuxParser::IdleJiffies());
  }
  std::this_thread::sleep_for(std::chrono::seconds(1));
  cpu_.value().setJiffies(LinuxParser::ActiveJiffies(), LinuxParser::IdleJiffies());
  return cpu_.value();
}

// DONE: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  if(!processes_.empty())
    processes_.clear();
  
  vector<int> pid = LinuxParser::Pids();
  
  for(const auto iP : pid){
    Process proc;
    proc.Pid(iP);
    proc.User(LinuxParser::User(iP));
    proc.Command(LinuxParser::Command(iP));
    proc.UpTime(LinuxParser::UpTime(iP));
    proc.Ram(LinuxParser::Ram(iP));
    proc.CpuUtilization(LinuxParser::ActiveJiffies(iP), LinuxParser::Jiffies());
    processes_.emplace_back(proc);
  }
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() { return LinuxParser::Kernel(); }

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

// DONE: Return the operating system name
std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() { return LinuxParser::UpTime(); }