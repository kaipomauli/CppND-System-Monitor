#include "processor.h"
#include <vector>
#include <string>
#include <thread>         // std::this_thread::sleep_for
#include <chrono>         // std::chrono::seconds



float Processor::Utilization() { 
    
    unsigned long long int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice,  PrevIdle, PrevNonIdle, PrevTotal, Idle_, NonIdle, Total,
        totald,idled;
    float CPU_Percentage;
	std::vector<std::string> cpudataStr = LinuxParser::CpuUtilization();
   
    std::vector<int> cpudata;
    
  for (auto i : cpudataStr) {
          cpudata.push_back(std::stoi(i));
  }
  
  user = cpudata[0];
  nice = cpudata[1];
  system = cpudata[2];
  idle = cpudata[3];
  iowait = cpudata[4];
  irq = cpudata[5];
  softirq = cpudata[6];
  steal = cpudata[7];
  //guest = cpudata[8];
  //guest_nice = cpudata[9];  

  PrevIdle = prev_idle + prev_iowait;
  Idle_ = idle + iowait;
  PrevNonIdle = prev_user + prev_nice + prev_system + prev_irq + prev_softirq + prev_steal;  
  NonIdle = user + nice + system + irq + softirq + steal;
  PrevTotal = PrevIdle + PrevNonIdle;
  Total = Idle_ + NonIdle;
  totald = Total - PrevTotal;
  idled = Idle_ - PrevIdle;
  CPU_Percentage = (totald - idled) / totald;

  prev_user = user;
  prev_nice = nice;
  prev_system = system;
  prev_irq = irq;
  prev_softirq = softirq;
  prev_steal = steal;
  prev_idle = idle;
  prev_iowait = iowait;

  return CPU_Percentage;
}