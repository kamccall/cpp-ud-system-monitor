#include <dirent.h>
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
float LinuxParser::MemoryUtilization() 
{ 
  string line;
  string x;    // dummy string for first entry on each line (can i avoid this like _ in python??) 
  string memTotal;
  string memFree;
  std::istringstream linestream;
  float memPercentUtilization;
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);  // grabs first (MemTotal) line
    linestream.str(line);
    linestream >> x >> memTotal;
    
    std::getline(filestream, line); // grabs second (MemFree) line
    linestream.str(line);
    linestream >> x >> memFree;
  }
  memPercentUtilization = (std::stof(memTotal) - std::stof(memFree)) / std::stof(memTotal);
  return memPercentUtilization;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() 
{ 
  string line;
  string uptime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    return std::stoi(uptime);
  }
  
  return 0;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() 
{ 
  long jiffiesTotal = 0;
  vector<string> cpuValues = CpuUtilization();
  
  for (string value: cpuValues)
  {
    jiffiesTotal += std::stol(value);
  }
  
  return jiffiesTotal;
}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) 
{ 
  string line, value;
  long uTime, sTime, cuTime, csTime;
  
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    vector<string> processValues {};
    
    for (int i = 0; i < 17; i++)
    {
      linestream >> value;
      processValues.emplace_back(value);
    }
    
    uTime     = std::stol(processValues[13]);
    sTime     = std::stol(processValues[14]);
    cuTime    = std::stol(processValues[15]);
    csTime    = std::stol(processValues[16]);
    
    return uTime + sTime + cuTime + csTime;
  }
  return 0;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return Jiffies() - IdleJiffies(); }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() 
{ 
  long jiffiesIdle = 0;
  vector<string> cpuValues = CpuUtilization();
  
  jiffiesIdle = std::stol(cpuValues[3]) + std::stol(cpuValues[4]);
  
  return jiffiesIdle;
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() 
{ 
  vector<string> procInfo = {};
  string line;
  string cpu, user, nice, system, idle, iowait, irq, softirq, steal;  // used names from stackoverflow article
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal;
    
    if (cpu == "cpu")
    {
      procInfo.push_back(user);
      procInfo.push_back(nice);
      procInfo.push_back(system);
      procInfo.push_back(idle);
      procInfo.push_back(iowait);
      procInfo.push_back(irq);
      procInfo.push_back(softirq);
      procInfo.push_back(steal);
      // next two values are included in prior values so don't need them
    }
    return procInfo;
  }
  return procInfo;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() 
{ 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes")
      {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() 
{ 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running")
      {
        return std::stoi(value);
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) 
{ 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    return line;
  }
  
  return string(); 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) 
{ 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "VmSize:") return value;
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) 
{ 
  // return string(); 
  string line;
  string key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "Uid:") return value;  //return userId; can use that to lookup name
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) 
{
  string line;
  string name, x, id;
  string userid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  
  if (filestream.is_open())
  {
    while (std::getline(filestream, line))
    {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> name >> x >> id;
      if (id == userid) return name;
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) 
{ 
  string line;
  string value;
  long seconds, uptimeProc;
  
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    
    for (int i = 1; i < 23; i++)
    {
      linestream >> value;  // will read into 'value' 22 times, then get last value
    }
    
    seconds    = std::stol(value) / sysconf(_SC_CLK_TCK);  // jiffies -> seconds
    uptimeProc = UpTime() - seconds;
    
    return uptimeProc;
  }
  
  return 0;
}
