
#include "system.h"



using std::set;
using std::size_t;
using std::string;
using std::vector;

/*You need to complete the mentioned TODOs in order to satisfy the rubric criteria "The student will be able to extract and display basic data about the system."

You need to properly format the uptime. Refer to the comments mentioned in format. cpp for formatting the uptime.*/


Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() {
  processes_.clear();
    for (int pid : LinuxParser::Pids()) {
      if (pid == 0) {
        std::cout << std::endl << "Process: pid = 0!!" << std::endl;
      }
      Process p(pid, LinuxParser::User(pid), LinuxParser::Ram(pid),
                LinuxParser::Command(pid), LinuxParser::UpTime(pid));
      p.setCpuUtilization();
      processes_.push_back(p);
    }
  
	std::sort(processes_.begin(), processes_.end(),[](Process a, Process b) {return b<a;});
	return processes_; 
}


std::string System::Kernel() { return LinuxParser::Kernel(); }


float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }


std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }


int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }


int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }


long int System::UpTime() { return LinuxParser::UpTime(); }
