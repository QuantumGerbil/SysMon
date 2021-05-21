#include "processor.h"
#include <limits>
#include <iostream>
#include <cmath>
 
// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  float deltaAct =  curActJiffies_ - prevActJiffies_;
  float deltaIdl =  curIdlJiffies_ - prevIdlJiffies_;
  float deltaTot = deltaAct + deltaIdl;
  if(deltaTot == 0)
    deltaTot = std::numeric_limits<float>::min();
  
  float retVal = deltaAct / deltaTot;
  //std::cerr << deltaAct << " " << deltaIdl << std::endl;
  return retVal;
}

void Processor::setJiffies(long active, long idle){
  prevActJiffies_ = curActJiffies_;
  prevIdlJiffies_ = curIdlJiffies_;
  
  curActJiffies_ = active;
  curIdlJiffies_ = idle;
  //std::cerr << "curActJiffies " << curActJiffies_ << " curIdlJiffies " << curIdlJiffies_ << " prevActJiffies " << prevActJiffies_ << " prevIdlJiffies " << prevIdlJiffies_ << std::endl;
}