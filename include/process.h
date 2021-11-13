#ifndef PROCESS_H
#define PROCESS_H
#include "linux_parser.h"
#include <string>
#include <thread>         // std::this_thread::sleep_for
#include <chrono> 
#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  int getPid();
  void setPid(int pid);                           // TODO: See src/process.cpp
  std::string getUser() { return user_; };  
  
  std::string Command();                   // TODO: See src/process.cpp
  float CpuUtilization() const { return util_; };  // TODO: See src/process.cpp
  void setCpuUtilization();
  std::string Ram();                       // TODO: See src/process.cpp
  long int UpTime();                       // TODO: See src/process.cpp
  bool operator<(Process& a);  // TODO: See src/process.cpp
  Process(int pid, std::string user, std::string ram, std::string cmd,long int proc_uptime)
      : pid_(pid),user_(user),ram_(ram),cmd_(cmd),proc_uptime_(proc_uptime){};
  // TODO: Declare any necessary private members
 private:
  int pid_=-1;
  std::string user_ = "";
  std::string ram_ = "";
  std::string cmd_ = "";
  long int proc_uptime_ = 0;
  float util_=-1.0;
  
  unsigned long int prev_utime = 0, prev_stime = 0, prev_cutime = 0,
                    prev_cstime = 0, prev_starttime = 0, prev_Sys_uptime = 0;
                    
};

#endif