#include <dirent.h>
#include <iomanip>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
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

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memoryUtilization = 0.0;
  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    string line1, line2;
    std::getline(stream, line1);
    std::getline(stream, line2);
    std::istringstream line1stream(line1);
    std::istringstream line2stream(line2);
    string memTotalString, memFreeString;
    float memTotal, memFree; // kB
    line1stream >> memTotalString >> memTotal;
    line2stream >> memFreeString >> memFree;
    memoryUtilization = (memTotal - memFree) / memTotal;
  }
  return memoryUtilization;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  long upTime = 0;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    string line;
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> upTime;
  }
  return upTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  long totalJiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    idle += iowait;
    long nonIdle = user + nice + system + irq + softirq + steal;
    totalJiffies = idle + nonIdle;
  }
  return totalJiffies;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) {
  long activeJiffies;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    std::istringstream linestream(line);
    string unused;
    for (int i = 0; i < 13; i++) {
      getline(linestream, unused, ' ');
    }
    string utimeString, stimeString, cutimeString, cstimeString;
    getline(linestream, utimeString, ' ');
    getline(linestream, stimeString, ' ');
    getline(linestream, cutimeString, ' ');
    getline(linestream, cstimeString, ' ');
    activeJiffies = std::stol(utimeString) + std::stol(stimeString) + std::stol(cutimeString) + std::stol(cstimeString);
  }
  return activeJiffies;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  long activeJiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    activeJiffies = user + nice + system + irq + softirq + steal;
  }
  return activeJiffies;
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  long idleJiffies;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    string cpu;
    long user, nice, system, idle, iowait;
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait;
    idleJiffies = idle + iowait;
  }
  return idleJiffies;
}

// TODO: Read and return CPU utilization
// I don't use the above functions in this one since that would read the stat file multiple times
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuUtilization;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    string cpu;
    long user, nice, system, idle, iowait, irq, softirq, steal;
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    idle += iowait;
    long nonIdle = user + nice + system + irq + softirq + steal;
    long total = idle + nonIdle;
    float utilization = (float)nonIdle / (float)total;
    cpuUtilization.push_back(to_string(utilization));
  }
  return cpuUtilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line, name;
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> name >> processes;
      if (name == kTotalProcessesLabel) {
        break;
      }
    }
  }
  return processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int processes = 0;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string line, name;
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> name >> processes;
      if (name == kRunningProcessesLabel) {
        break;
      }
    }
  }
  return processes;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    getline(stream, line);
  }
  // We make every command 256 characters for formatting reasons
  while (line.size() < 256) {
    line += " ";
  }
  return line;
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {
  float ram = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string line, label, ramkB;
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> label >> ramkB;
      if (label == kRamLabel) {
        ram = stof(ramkB);
        ram /= 1000; // converting to MB
        break;
      }
    }
  }
  return to_string(ram).substr(0, 5);
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  string uid;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    string line, label;
    while(getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> label >> uid;
      if (label == kUidLabel) {
        break;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string username;
  string uid = LinuxParser::Uid(pid);
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    string line, x, num;
    while (getline(stream, line)) {
      std::istringstream linestream(line);
      getline(linestream, username, ':');
      getline(linestream, x, ':');
      getline(linestream, num, ':');
      if (num == uid) {
        break;
      }
    }
  }
  // we make the username have 5 characters for formatting reasons
  if (username.size() < 5) {
    std::stringstream stringstream;
    stringstream << std::setw(5) << username;
    username = stringstream.str();
  } else if (username.size() > 5) {
    username = username.substr(0, 5);
  }
  return username;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long upTime = 0;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    string line, unused, startTimeString;
    getline(stream, line);
    std::istringstream linestream(line);
    for (int i = 0; i < 21; i++) {
      getline(linestream, unused, ' ');
    }
    getline(linestream, startTimeString, ' ');
    long processStartTime = std::stol(startTimeString) / sysconf(_SC_CLK_TCK);
    upTime = LinuxParser::UpTime() - processStartTime;
  }
  return upTime;
}
