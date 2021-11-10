#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <fstream>
#include <regex>
#include <string>
#include <filesystem> 
#include <iostream>
namespace fs = std::filesystem;

namespace LinuxParser {
// Paths
const fs::path kProcDirectory("/proc");
const fs::path kCmdlineFilename{"cmdline"};
const fs::path kCpuinfoFilename{"cpuinfo"};
const fs::path kStatusFilename{"status"};
const fs::path kStatFilename{"stat"};
const fs::path kUptimeFilename{"uptime"};
const fs::path kMeminfoFilename{"meminfo"};
const fs::path kVersionFilename{"version"};
const fs::path kOSPath {"/etc/os-release"};
const fs::path kPasswordPath{"/etc/passwd"};

// System
float MemoryUtilization();
long UpTime();
std::vector<int> Pids();
int TotalProcesses();
int RunningProcesses();
std::string OperatingSystem();
std::string Kernel();

// CPU
enum CPUStates {
  kUser_ = 0,
  kNice_,
  kSystem_,
  kIdle_,
  kIOwait_,
  kIRQ_,
  kSoftIRQ_,
  kSteal_,
  kGuest_,
  kGuestNice_
};
std::vector<unsigned long> CpuUtil(int pid);
std::vector<std::string> CpuUtilization();
long Jiffies();
long ActiveJiffies();
long ActiveJiffies(int pid);
long IdleJiffies();

// Processes
std::string Command(int pid);
std::string Ram(int pid);
std::string Uid(int pid);
std::string User(int pid);
long int UpTime(int pid);
};  // namespace LinuxParser

#endif