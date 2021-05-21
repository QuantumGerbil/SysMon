#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  Processor(long active, long idle): curActJiffies_(active), curIdlJiffies_(idle){};
  float Utilization();  // TODO: See src/processor.cpp
  void setJiffies(long active, long idle);

  // TODO: Declare any necessary private members
 private:
  long curActJiffies_;
  long curIdlJiffies_;
  long prevActJiffies_;
  long prevIdlJiffies_;
};

#endif