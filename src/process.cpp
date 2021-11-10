#include "process.h"

using std::string;
using std::to_string;
using std::vector;


int Process::getPid() { return pid_; }


// TODO: Return this process's CPU utilization
void Process::setCpuUtilization() { 
    unsigned long int utime, stime, cutime, cstime, starttime,TotTime,
      PrevTotTime, PrevSeconds, Seconds,  totald, secsld,Sys_uptime;
        
    vector<unsigned long> new_data = LinuxParser::CpuUtil(pid_);
    Sys_uptime = LinuxParser::UpTime();
    

    utime = new_data[0];
    stime = new_data[1];
    cutime = new_data[2];
    cstime = new_data[3];
    starttime = new_data[4];

    PrevTotTime = prev_utime + prev_stime + prev_cutime + prev_cstime;
    TotTime = utime + stime + cutime + cstime;
    totald = TotTime - PrevTotTime;
    PrevSeconds = prev_Sys_uptime - prev_starttime;
    Seconds = Sys_uptime - starttime;
    secsld = Seconds - PrevSeconds;

    prev_utime = utime;
    prev_stime = stime;
    prev_cutime = cutime;
    prev_cstime = cstime;
    prev_starttime = starttime;
    prev_Sys_uptime = Sys_uptime;

    float cpuPercentage = secsld > 0 ? float(totald) / float(secsld):0.0;
    util_ = cpuPercentage;
    
}


string Process::Command() { return cmd_; }


string Process::Ram() { return ram_; }


  
long int Process::UpTime() { return proc_uptime_; }


bool Process::operator<(Process& a ) { 
    bool cpuEqu = this->CpuUtilization() == a.CpuUtilization();
  float ramThis = std::stof(this->Ram());
    float ram_a = std::stof(a.Ram());
  bool ramSmOEqu = ramThis <= ram_a;
    bool cpuSmaller = this->CpuUtilization() < a.CpuUtilization();
  bool finalAns = cpuEqu ? ramSmOEqu : cpuSmaller;
      return finalAns;
             
}

