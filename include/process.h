#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int Pid();                               // TODO: See src/process.cpp
  void Pid(int const p);
  std::string User();                      // TODO: See src/process.cpp
  void User(std::string const st);
  std::string Command();                   // TODO: See src/process.cpp
  void Command(std::string const st);
  float CpuUtilization();                  // TODO: See src/process.cpp
  void CpuUtilization(long const activeJiffies, long const totalJiffies);
  std::string Ram();                       // TODO: See src/process.cpp
  void Ram(std::string const st);
  long int UpTime();                       // TODO: See src/process.cpp
  void UpTime(long const l);
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // DONE: Declare any necessary private members
 private:
  int pid_{0};
  std::string user_;
  std::string cmd_;
  long upTime_;
  float cpu_{0};
  float previousJiffies_{0};
  float prevTotalJiffies_{0};
  std::string ram_;
};

#endif