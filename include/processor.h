#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h" 
class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  unsigned long int prev_user=0, prev_nice=0, prev_system=0, prev_idle=0, prev_iowait=0,
      prev_irq=0, prev_softirq=0, prev_steal=0, prev_guest=0, prev_guest_nice=0;
};

#endif