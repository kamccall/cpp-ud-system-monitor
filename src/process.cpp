#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// TODO: Return this process's ID
int Process::Pid() const
{ 
  return pid_;
}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() 
{ 
  float processUtil = 0.0;
  long processJiffies = LinuxParser::ActiveJiffies(pid_);  // returns jiffies
  long processUptime  = LinuxParser::UpTime(pid_);         // returns seconds
  
  processUtil = float(processJiffies) / sysconf(_SC_CLK_TCK) / float(processUptime);
  
  return processUtil;
}

// KM ADDED - added to private members to enable predictable sort every refresh
float Process::GetCpu() const
{
  return cpu_;
}

void Process::SetCpu(float value)
{
  if (value >= 0.0) cpu_ = value;
  	else cpu_ = 0.0;
}

// TODO: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// TODO: Return this process's memory utilization
string Process::Ram() 
{ 
  string processMemory = LinuxParser::Ram(pid_);
  
  int processMemInt = std::stoi(processMemory);
  processMemInt /= 1024;        // convert kilobytes -> megabytes
  
  return std::to_string(processMemInt); 
}

// TODO: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { return a.GetCpu() < this->GetCpu(); }