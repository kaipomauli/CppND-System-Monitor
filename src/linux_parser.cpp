#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;


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
  }
  return value;
}


string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory / kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}



// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
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
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  string unit;
  float memfree;
  float memtotal;
  float memUtil;
  bool memfreeSet = false;
  bool memtotalSet = false;
  fs::path fullpath = kProcDirectory;
  fullpath /= kMeminfoFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)){
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    while (std::getline(filestream, line) && (!memfreeSet || !memtotalSet)) {
      
      std::replace(line.begin(), line.end(), ':', ' ');
      
      std::istringstream linestream(line);
      while (linestream >> key >> value >>unit ) {
        if (key == "MemFree") {
          
          memfree=std::stof(value);
          memfreeSet = true;
        }
        if (key == "MemTotal") {
         
          memtotal = std::stof(value);
          memtotalSet = true;
        }
      }
    }
  }
  memUtil = (memtotal - memfree) / memtotal;
  return memUtil; 
}


long LinuxParser::UpTime() { 
    string line;
  string key;
  string value;
  long uptime;

  fs::path fullpath = kProcDirectory / kUptimeFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    if(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;

      uptime = std::stoi(key);
      return uptime;     
    } else {
      std::cout << "Could not get line." << std::endl;
    }
  } else {
    std::cout << "Could not open file." << std::endl;
  }
  
    return uptime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }

vector<unsigned long> LinuxParser::CpuUtil(int pid) {
  string line;
  string key;
  vector<unsigned long> statInt;
  vector<string> statData;
  fs::path pidPath(std::to_string(pid));
  fs::path fullpath = kProcDirectory / pidPath / kStatFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << std::endl<< "CPUUtil: Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    std::getline(filestream, line);

    std::istringstream linestream(line);
    while (linestream >> key) {
      statData.push_back(key);
    }

  } else {
    std::cout << std::endl << "CPUUtil: Could not open file." << std::endl;
  }
  statInt.push_back(std::stoi(statData[13]) / sysconf(_SC_CLK_TCK));
  statInt.push_back(std::stoi(statData[14]) / sysconf(_SC_CLK_TCK));
  statInt.push_back(std::stoi(statData[15]) / sysconf(_SC_CLK_TCK));
  statInt.push_back(std::stoi(statData[16]) / sysconf(_SC_CLK_TCK));
  statInt.push_back(std::stoi(statData[21]) / sysconf(_SC_CLK_TCK));


  return statInt;
}
vector<string> LinuxParser::CpuUtilization() { 
   vector<string> cpuData;
  string line;
  string key;
  
  

  fs::path fullpath = kProcDirectory / kStatFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    if (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key) {
        cpuData.push_back(key);
      }

      
    } else {
      std::cout << "Could not get line." << std::endl;
    }
  } else {
    std::cout << "Could not open file." << std::endl;
  }
  assert(cpuData[0] == "cpu");
  cpuData.erase(cpuData.begin());
  return cpuData; 
}


int LinuxParser::TotalProcesses() {
  string line;
  string key;
  string value;
  int proc;
  

  fs::path fullpath = kProcDirectory / kStatFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      
      if (key == "processes") {
          proc = std::stoi(value);
          return proc;
      } else {
        continue;
      }
      
    }
  }
 
  return proc;
}


int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  int proc;

  fs::path fullpath = kProcDirectory / kStatFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;

      if (key == "procs_running") {
        proc = std::stoi(value);
        return proc;
      } else {
        continue;
      }
    }
  }
  return proc;
}


string LinuxParser::Command(int pid) { 
  string cmd;
  string key;
  string uid;

  fs::path pidPath(std::to_string(pid));
  fs::path fullpath = kProcDirectory / pidPath / kCmdlineFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }

  if (filestream.is_open()) {
       if(std::getline(filestream, cmd)) {
          return cmd;
       }
       else {
         std::cout << "Could not get line." << std::endl;
       }
  } else {
    std::cout << "Could not open file." << std::endl;
  }
  return cmd;
}


string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string ramStr;

  fs::path pidPath(std::to_string(pid));
  fs::path fullpath = kProcDirectory / pidPath / kStatusFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> ramStr;
        break;
      }
    }
  } else {
    std::cout << "Could not open file." << std::endl;
  }
  unsigned int ram_kb = std::stoi(ramStr);
  float ram_Mb = ram_kb / 1024.0;
  return to_string(ram_Mb).substr(0, 5);
}


string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string uid;

  fs::path pidPath(std::to_string(pid));
  fs::path fullpath = kProcDirectory / pidPath / kStatusFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  } else {
    std::cout << "Could not open file." << std::endl;
  }
    return uid; 
}


string LinuxParser::User(int pid) { 
  
  string line2;
  string user;
  string x;
  string id;
  string ud = Uid(pid);
  std::ifstream f(kPasswordPath);
  if (!fs::exists(kPasswordPath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (f.is_open()) {
    while (std::getline(f, line2)) {
      std::replace(line2.begin(), line2.end(), ':', ' ');
      std::istringstream l(line2);
      l >> user >>x>>id;
      if (id == ud) {
        return user;
        
      }
    }
  } else {
    std::cout << "Could not open file." << std::endl;
  }
  
  return user;
}


long LinuxParser::UpTime(int pid) { 
    string line;
  string key;
  
  vector<string> statData;
  fs::path pidPath(std::to_string(pid));
  fs::path fullpath = kProcDirectory / pidPath / kStatFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "PID Uptime: Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    std::getline(filestream, line);
    
      std::istringstream linestream(line);
    while (linestream >> key) {
        statData.push_back(key);
    }
    
    
  } else {
    std::cout << "PID Uptime: Could not open file." << std::endl;
  }
  long int ut = std::stoi(statData[21])/sysconf(_SC_CLK_TCK);
  return ut;
}
