#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <cassert>
#include "linux_parser.h"
#include <iostream>
#include <string>
#include <algorithm>




using std::stof;
using std::string;
using std::to_string;
using std::vector;

std::string LinuxParser::ltrim(const std::string& s) {
  size_t start = s.find_first_not_of(WHITESPACE);
  return (start == std::string::npos) ? std::string("") : s.substr(start);
}

std::string LinuxParser::rtrim(const std::string& s) {
  size_t end = s.find_last_not_of(WHITESPACE);
  return (end == std::string::npos) ? std::string("") : s.substr(0, end + 1);
}

std::string LinuxParser::trim(const std::string& s) { return rtrim(ltrim(s)); }


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
        int pid = stoi(trim(filename));
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
  float memfree=0.0;
  float memtotal=0.0;
  float memUtil=0.0;
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
        try{
          memfree=std::stof(trim(value));
        }
        catch (std::invalid_argument& e) {
          std::cout << "fault in stof in system Mem Utilization" << std::endl;
        }
          memfreeSet = true;
        }
        if (key == "MemTotal") {
         try{
          memtotal = std::stof(trim(value));
        }
        catch (std::invalid_argument& e) {
          std::cout << "fault in stof in system Mem Utilization" << std::endl;
        }
          memtotalSet = true;
        }
      }
    }
  }
  memUtil = (memtotal - memfree) / memtotal;
  return memUtil; 
}


long int LinuxParser::UpTime() { 
    string line;
  string key;
  string value;
  long int uptime=0;

  fs::path fullpath = kProcDirectory / kUptimeFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << "Path does not exist" << std::endl;
  }
  if (filestream.is_open()) {
    if(std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
    try{
      uptime = std::stoi(trim(key));
    }
    catch (std::invalid_argument& e) {
      std::cout << "fault in stoi in System uptime" << std::endl;
    }
      return uptime;     
    } else {
      std::cout << "Could not get line." << std::endl;
    }
  } else {
    std::cout << "Could not open file." << std::endl;
  }
  
    return uptime;
}

vector<unsigned long int> LinuxParser::CpuUtil(int pid) {
  string line;
  string key;
  vector<unsigned long int> statInt;
  vector<string> statData;
  fs::path pidPath(std::to_string(pid));
  fs::path fullpath = kProcDirectory / pidPath / kStatFilename;
  std::ifstream filestream(fullpath);
  if (!fs::exists(fullpath)) {
    std::cout << std::endl << "CPUUtil: Path does not exist" << std::endl;
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
  try{
      statInt.push_back(std::stoi(trim(statData[13])) / sysconf(_SC_CLK_TCK));
      statInt.push_back(std::stoi(trim(statData[14])) / sysconf(_SC_CLK_TCK));
      statInt.push_back(std::stoi(trim(statData[15])) / sysconf(_SC_CLK_TCK));
      statInt.push_back(std::stoi(trim(statData[16])) / sysconf(_SC_CLK_TCK));
      statInt.push_back(std::stoi(trim(statData[21])) / sysconf(_SC_CLK_TCK));
   }catch (std::invalid_argument& e) {
    std::cout << "fault in stoi in CpuUtil with pid" << std::endl;
    }

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
  int proc=0;
  

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
          try{
            proc = std::stoi(trim(value));
           } catch (std::invalid_argument& e) {
          std::cout << "fault in stoi in TotalProcesses" << std::endl;
        }
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
  int proc=0;

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
        try{
            proc = std::stoi(trim(value));
        } catch (std::invalid_argument& e) {
          std::cout << "fault in stoi in Running processes" << std::endl;
        }
      
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
  unsigned int ram_kb=0;
  try {
    ram_kb = std::stoi(trim(ramStr));
  } catch (std::invalid_argument& e) {
    std::cout << "fault in stoi in RAM with pid" << std::endl;
  }
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


long int LinuxParser::UpTime(int pid) { 
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
  long int ut=0;
  try {
  ut= std::stoi(trim(statData[21])) / sysconf(_SC_CLK_TCK);
  } catch (std::invalid_argument& e) {
    std::cout << "fault in stoi in Uptime with pid" << std::endl;
  }
  return LinuxParser::UpTime()-ut;
}
