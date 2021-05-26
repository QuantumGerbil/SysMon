#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include "linux_parser.h"
#include <bits/stdc++.h>


#include "process.h"
#include "processor.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// Done: Return the system's CPU
Processor& System::Cpu() {
  static long runTime = upTime_;
  if(!cpu_){
    cpu_ = Processor(LinuxParser::ActiveJiffies(), LinuxParser::IdleJiffies());
  }
  
  if(runTime < upTime_){	//since upTime is in seconds limits update to once a second without blocking
    runTime = upTime_;
    cpu_.value().setJiffies(LinuxParser::ActiveJiffies(), LinuxParser::IdleJiffies());
  }
  
  return cpu_.value();
}

// DONE: Return a container composed of the system's processes
// In an attempt to avoid file errors due to too many calls to open I'm restricting the full rebuild of the process vector
// to once on the first call to the function, and if the number of total processes changes. Otherwise updates of the processes is time synced
vector<Process>& System::Processes() {
  static long runTime = upTime_;
  if(processes_.empty()){  
  	vector<int> pid = LinuxParser::Pids();
    int jif = LinuxParser::Jiffies();
  	for(const auto iP : pid){
    	Process proc;
    	proc.Pid(iP);
    	proc.User(LinuxParser::User(iP));
    	proc.Command(LinuxParser::Command(iP));
    	proc.UpTime(LinuxParser::UpTime(iP));
    	proc.Ram(LinuxParser::Ram(iP));
    	proc.CpuUtilization(LinuxParser::ActiveJiffies(iP), jif);
    	processes_.emplace_back(proc);
  	}
  }else{
    if(runTime < upTime_){
      runTime = upTime_;
      int jif = LinuxParser::Jiffies();
      vector<int> pid = LinuxParser::Pids();
      if(pid.size() != processes_.size()){
  	    for(const auto iP : pid){
    	  Process proc;
    	  proc.Pid(iP);
    	  proc.User(LinuxParser::User(iP));
    	  proc.Command(LinuxParser::Command(iP));
    	  proc.UpTime(LinuxParser::UpTime(iP));
    	  proc.Ram(LinuxParser::Ram(iP));
    	  proc.CpuUtilization(LinuxParser::ActiveJiffies(iP), jif);
    	  processes_.emplace_back(proc);
  	    }
      }else{      
        for(auto process : processes_){
          int pid = process.Pid();
          process.UpTime(LinuxParser::UpTime(pid));
          process.Ram(LinuxParser::Ram(pid));
          process.CpuUtilization(LinuxParser::ActiveJiffies(pid), jif);
        }
      }
    }
  }
  std::sort(processes_.begin(), processes_.end());
  return processes_;
}

// DONE: Return the system's kernel identifier (string)
std::string System::Kernel() {
  if(!kernel_)
	kernel_.emplace(LinuxParser::Kernel());
  
  return kernel_.value_or("Unknown");
}

// DONE: Return the system's memory utilization
float System::MemoryUtilization() { 
  if(!memUtilization_){
    memUtilization_.emplace(std::make_pair(upTime_, LinuxParser::MemoryUtilization()));
  }
  
  if(memUtilization_.value().first < upTime_){
    memUtilization_.emplace(std::make_pair(upTime_, LinuxParser::MemoryUtilization()));
  }
    
  return memUtilization_.value().second;
}

// DONE: Return the operating system name
std::string System::OperatingSystem() {
  if(!os_){
    os_ = LinuxParser::OperatingSystem();
  }
  return os_.value_or("Unknown");
}

// DONE: Return the number of processes actively running on the system
int System::RunningProcesses() {
  if(!runningProcCount_){
    runningProcCount_.emplace(std::make_pair(upTime_, LinuxParser::RunningProcesses()));
  }
  
  if(runningProcCount_.value().first < upTime_){
    runningProcCount_.emplace(std::make_pair(upTime_, LinuxParser::RunningProcesses()));
  }
    
  return runningProcCount_.value().second;
 }

// DONE: Return the total number of processes on the system
int System::TotalProcesses() {
  if(!totalProcCount_){
    totalProcCount_.emplace(std::make_pair(upTime_, LinuxParser::TotalProcesses()));
  }
  
  if(totalProcCount_.value().first < upTime_){
    totalProcCount_.emplace(std::make_pair(upTime_, LinuxParser::TotalProcesses()));
  }
    
    return totalProcCount_.value().second;
  }

// DONE: Return the number of seconds since the system started running
long int System::UpTime() {
  upTime_ = LinuxParser::UpTime();
  return upTime_;
}