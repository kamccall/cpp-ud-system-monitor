#include "processor.h"
#include "linux_parser.h"
#include <string>
using namespace std;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() 
{ 
  long timeActive = LinuxParser::ActiveJiffies();
  long timeTotal  = LinuxParser::Jiffies();
  
  float diffActive = timeActive - previousActive;
  float diffTotal  = timeTotal - previousTotal;
  
  float cpuUtil   = diffActive / diffTotal;
  
  previousActive  = timeActive;
  previousTotal   = timeTotal;
  
  return cpuUtil;
}