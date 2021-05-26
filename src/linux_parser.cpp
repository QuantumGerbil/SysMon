#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>
#include <experimental/optional>
#include <limits>
#include <numeric>
#include <experimental/filesystem>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using std::experimental::optional;

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
  } else{
    throw std::runtime_error("string LinuxParser::OperatingSystem():File doesn't exist or can't open");
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  } else{
    throw std::runtime_error("string LinuxParser::Kernel():File doesn't exist or can't open");
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  string path = kProcDirectory;
  for (const auto &file : std::experimental::filesystem::directory_iterator(path)){
    if(std::experimental::filesystem::is_directory(file)){
      string filename(file.path().filename());
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
          
  /*DIR* directory = opendir(kProcDirectory.c_str());
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
  closedir(directory);*/
  
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  optional<float> retVal;
  string key, buffer;
  vector<float> memInfoProc;
  std::ifstream fileStream(kProcDirectory + kMeminfoFilename);
  if (fileStream.is_open()) {
    for (int i = 0; i < 22;i++) {  // Only interested in first 5 data points from /proc/meminfo
       	std::getline(fileStream, buffer);
        if((i < 2)||((i > 2)&&(i < 5))||i == 21){
      		std::istringstream ss(buffer);
        	float value;
          	ss >> key >> value;
      		memInfoProc.push_back(value);
      		ss.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
      	}
    }
    retVal = (memInfoProc[MemInfo::MemTotal] - memInfoProc[MemInfo::MemFree] - memInfoProc[MemInfo::MemBuffer] -
              memInfoProc[MemInfo::MemCached] - memInfoProc[MemInfo::MemSlab]) / memInfoProc[MemInfo::MemTotal];
  } else{
    throw std::runtime_error("float LinuxParser::MemoryUtilization():File doesn't exist or can't open");
  }  
  return retVal.value_or(-1);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  optional<long> retVal;
  string buffer;
  std::ifstream fileStream(kProcDirectory + kUptimeFilename);
  if (fileStream.is_open()) {
    long value;
    std::getline(fileStream, buffer);
    std::istringstream ss(buffer);
    ss >> value;
    retVal.emplace(value);
  } else{
    throw std::runtime_error("long LinuxParser::UpTime():File doesn't exist or can't open");
  }
  return retVal.value_or(-1);
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return ActiveJiffies() + IdleJiffies();
}

/**
Table 1-3: Contents of the stat files (as of 2.6.22-rc3)
..............................................................................
 Field          Content
  pid           process id
  tcomm         filename of the executable
  state         state (R is running, S is sleeping, D is sleeping in an
                uninterruptible wait, Z is zombie, T is traced or stopped)
  ppid          process id of the parent process
  pgrp          pgrp of the process
  sid           session id
  tty_nr        tty the process uses
  tty_pgrp      pgrp of the tty
  flags         task flags
  min_flt       number of minor faults
  cmin_flt      number of minor faults with child's
  maj_flt       number of major faults
  cmaj_flt      number of major faults with child's
  utime         user mode jiffies
  stime         kernel mode jiffies
  cutime        user mode jiffies with child's
  cstime        kernel mode jiffies with child's
**/

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long retVal = 0;
  string buffer;
  vector<long> statCPUProc;
  std::string path = kProcDirectory + std::to_string(pid) + "/stat";
  std::ifstream fileStream(path);
  if (fileStream.is_open()) {
    string ignore;
    std::getline(fileStream, buffer);
    std::istringstream ss(buffer);

    for(int i = 0; i < 13; i++){
      ss >> ignore;
    }
    long time = 0;
    for(int i = 0; i < 2; i++){
    	ss >> time;
      	retVal += time;
    }
  } else{
    throw std::runtime_error("long LinuxParser::ActiveJiffies(int pid):" + path + " doesn't exist or can't open");
  }
  return retVal;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  optional<long> retVal;
  string buffer;
  vector<long> statCPUProc;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    long value;
    string ignore;
    std::getline(fileStream, buffer);
    std::istringstream ss(buffer);
    ss >> ignore;
    for(int i = 0; i < 10; i++){
      ss >> value;
      statCPUProc.push_back(value);
    }
    retVal = statCPUProc[CPUStates::kUser_] + statCPUProc[CPUStates::kNice_] + statCPUProc[CPUStates::kSystem_]
               + statCPUProc[CPUStates::kSoftIRQ_] + statCPUProc[CPUStates::kIRQ_] + statCPUProc[CPUStates::kSteal_]
               + statCPUProc[CPUStates::kGuest_] + statCPUProc[CPUStates::kGuestNice_];
  } else{
    throw std::runtime_error("long LinuxParser::ActiveJiffies():File doesn't exist or can't open");
  }
  //std::cerr << "long LinuxParser::ActiveJiffies(): " << *retVal << std::endl;
  return retVal.value_or(-1);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  optional<long> retVal;
  string buffer;
  vector<long> statCPUProc;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    long value;
    string ignore;
    std::getline(fileStream, buffer);
    std::istringstream ss(buffer);
    ss >> ignore;
    for(int i = 0; i < 10; i++){
      ss >> value;
      statCPUProc.push_back(value);
    }
    retVal = statCPUProc[CPUStates::kIdle_] + statCPUProc[CPUStates::kIOwait_];
  } else{
    throw std::runtime_error("long LinuxParser::IdleJiffies():File doesn't exist or can't open");
  }
  //std::cerr << "long LinuxParser::IdleJiffies(): " << *retVal << std::endl;
  return retVal.value_or(-1);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  optional<vector<string> > retVal;
  string buffer;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    vector<long> statCPUProc;
    long value;
    string ignore;
    std::getline(fileStream, buffer);
    while(buffer.substr(0, 3) == "cpu"){
    	std::istringstream ss(buffer);
    	ss >> ignore;
    	for(int i = 0; i < 10; i++){
      		ss >> value;
      		statCPUProc.push_back(value);
    	}
    	long total = std::accumulate(statCPUProc.begin(), statCPUProc.end(), 0);
      	string lToS = std::to_string(total);
      	retVal.value().push_back(lToS);
        std::getline(fileStream, buffer);
    }  
  } else{
    throw std::runtime_error("vector<string> LinuxParser::CpuUtilization():File doesn't exist or can't open");
  }
  return retVal.value_or(vector<string>());
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  optional<int> retVal;
  string key, buffer;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    float value = 0;
    while (std::getline(fileStream, buffer)) {
      std::istringstream ss(buffer);
      while (ss >> key >> value) {
        if (key == "processes") {
          retVal = value;
        }
      }
    }
  } else{
    throw std::runtime_error("int LinuxParser::TotalProcesses():File doesn't exist or can't open");
  }
  return retVal.value_or(-1);
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  optional<int> retVal;
  string key, buffer;
  std::ifstream fileStream(kProcDirectory + kStatFilename);
  if (fileStream.is_open()) {
    float value = 0;
    while (std::getline(fileStream, buffer)) {
      std::istringstream ss(buffer);
      while (ss >> key >> value) {
        if (key == "procs_running") {
          retVal = value;
        }
      }
    }
  } else{
    throw std::runtime_error("int LinuxParser::RunningProcesses():File doesn't exist or can't open");
  }
  return retVal.value_or(-1);
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  optional<string> retVal;
  string buffer;
  string path = kProcDirectory + std::to_string(pid) + "/cmdline";
  std::ifstream fileStream(path);
  if (fileStream.is_open()) {
    std::getline(fileStream, buffer);
    retVal = buffer;
  } else{
    throw std::runtime_error("string LinuxParser::Command(int pid):File doesn't exist or can't open");
  }
  return retVal.value_or("N/A");
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  optional<string> retVal;
  string buffer, key;
  long value;
  string path = kProcDirectory + std::to_string(pid) + "/status";
  std::ifstream fileStream(path);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, buffer)) {
      std::istringstream ss(buffer);
      while (ss >> key >> value) {
        if (key == "VmSize:") {
          retVal = std::to_string(value / 1000);	//kb to mb
        }
      }
    }
  } else{
    throw std::runtime_error("string LinuxParser::Ram(int pid):File doesn't exist or can't open");
  }
  return retVal.value_or("N/A");
}

// DONE: Read and return the user ID associated with a process
// TODO: Keeps throwing exception
string LinuxParser::Uid(int pid) {
  optional<string> retVal;
  string buffer, key;
  long value;
  string path = kProcDirectory + std::to_string(pid) + "/status";
  std::ifstream fileStream(path);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, buffer)) {
      std::istringstream ss(buffer);
      while (ss >> key >> value) {
        if (key == "Uid:") {
          retVal = std::to_string(value);
        }
      }
    }
  } else{
    throw std::runtime_error("string LinuxParser::Uid(int pid):File doesn't exist or can't open");
  }
  return retVal.value();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  optional<string> retVal;
  string uid;
  uid = LinuxParser::Uid(pid);
  string buffer, key, value;

  std::ifstream fileStream(kPasswordPath);
  if (fileStream.is_open()) {
    while (std::getline(fileStream, buffer)) {
      std::replace( buffer.begin(), buffer.end(), ':', ' ');
      std::replace( buffer.begin(), buffer.end(), ',', ' ');
      std::istringstream ss(buffer);
      while (ss >> value >> key >> key) {
        if (key == uid) {
          retVal.emplace(value);
        }
      }
    }
  } else{
    throw std::runtime_error("string LinuxParser::User(int pid):File doesn't exist or can't open");
  }
  return retVal.value_or("N/A");
}

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  optional<long> retVal;
  int Hz = sysconf(_SC_CLK_TCK);
  long uptime = LinuxParser::UpTime();
  long processStartTime;
  
  string buffer, key;
  string path = kProcDirectory + std::to_string(pid) + "/stat";
  std::ifstream fileStream(path);
  if (fileStream.is_open()) {
    std::getline(fileStream, buffer);
    std::istringstream ss(buffer);
    for(int i = 0; i < 22; i++){
      ss >> processStartTime;
    }
    retVal = uptime - processStartTime /Hz;
  } else{
    throw std::runtime_error("long LinuxParser::UpTime(int pid):File doesn't exist or can't open");
  }
  return retVal.value_or(-1);  
}
