#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) : pid_(pid) {};

// TODO: Return this process's ID
int Process::Pid() { return this->pid_; }

// TODO: Return this process's CPU utilization
// This doesn't use the LinuxParser::ActiveJiffies(pid) and LinuxParser::UpTime(pid) functions to 
// avoid reading the same file twice
float Process::CpuUtilization() const {
  float cpuUtilization = 0;
  std::ifstream stream(LinuxParser::kProcDirectory + to_string(this->pid_) + LinuxParser::kStatFilename);
  if (stream.is_open()) {
    string line;
    getline(stream, line);
    std::istringstream linestream(line);
    string unused;
    for (int i = 0; i < 13; i++) {
      getline(linestream, unused, ' ');
    }
    string utimeString, stimeString, cutimeString, cstimeString, starttimeString;
    getline(linestream, utimeString, ' ');
    getline(linestream, stimeString, ' ');
    getline(linestream, cutimeString, ' ');
    getline(linestream, cstimeString, ' ');
    for (int i = 0; i < 4; i++) {
      getline(linestream, unused, ' ');
    }
    getline(linestream, starttimeString, ' ');
    long totalTime = std::stol(utimeString) + std::stol(stimeString) + std::stol(cutimeString) + std::stol(cstimeString);
    long seconds = LinuxParser::UpTime() - (std::stol(starttimeString) / sysconf(_SC_CLK_TCK));
    cpuUtilization = ((float)totalTime / sysconf(_SC_CLK_TCK)) / (float)seconds;
  }
  return cpuUtilization;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(this->pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(this->pid_); }

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(this->pid_); }

// TODO: Return the age of this process (in seconds)
long Process::UpTime() { return LinuxParser::UpTime(this->pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {
  return this->CpuUtilization() < a.CpuUtilization();
}